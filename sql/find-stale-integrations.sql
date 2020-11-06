
-- Increase statement timeout for this session only.
BEGIN;
SET statement_timeout = '24h';
COMMIT;


/**
 * This complex query outputs all "stale" OpenFaas integration configurations.
 *
 * An integration configuration is considered "stale" if it is:
 *   - neither referenced by any published module
 *   - nor by any "recent" unpublished module version.
 *
 * "Recentness" is defined as "3 versions or 1 month back" and can be adjusted
 * in the `fresh_active_integrations_in_unpublished` subquery.
 *
 * If you plan to use this script for cleaning up unused integrations, for each
 * candidate please check its creation date (to avoid removing created but not
 * yet used integrations) and run `find-integration-references.sql` to confirm
 * "staleness".
 *
 * NOTE: Requires `to_external_id()` and `from_external_id()` functions.
 *
 * NOTE: As of Nov 2020, this query takes more than 7 hours. Consider running
 * it on the read-only replica overnight.
 */
WITH
/**
 * Fetches info on integration nodes from all ever published
 * and not yet deleted modules with old-style integration key.
 *
 * NOTE: Use `published_modules.type <> TEST` WHERE clause to exclude
 * TEST published modules.
 */
  active_integrations_in_published AS (
  SELECT
    published_modules.name AS module_name,
    json_data.value->>'label' AS node_label,
    json_data.value->>'integrationKey' AS integration_key,
    published_modules.created_at
  FROM
    published_modules,
    -- Without `regexp_replace` postgres chokes on a few module JSONs.
    json_each(regexp_replace(data, '\\u0000', '', 'g')::JSON->'nodes') AS json_data
  WHERE
    published_modules.deleted = FALSE AND
    json_typeof(regexp_replace(data, '\\u0000', '', 'g')::JSON->'nodes') = 'object' AND
    json_data.value->'integrationKey' IS NOT NULL AND
    position('$$' in json_data.value->>'integrationKey') <> 0
),
/**
 * Fetches info on integration nodes from all module versions of
 * not yet deleted modules with old-style integration key.
 *
 * NOTE: This query is particularly expensive because it parses JSON
 * data for the largest table in the database. On `prod` as of Nov 2020
 * it already runs for about 7 hours.
 */
  active_integrations_in_unpublished AS (
  SELECT
    module_versions.name AS module_name,
    json_data.value->>'label' AS node_label,
    json_data.value->>'integrationKey' AS integration_key,
    module_versions.created_at,
    module_versions.version,
    modules.latest_version
  FROM
    module_versions,
    modules,
    -- Without `regexp_replace` postgres chokes on a few module JSONs.
    json_each(regexp_replace(data, '\\u0000', '', 'g')::JSON->'nodes') AS json_data
  WHERE
    module_versions.id = modules.module_id AND
    modules.deleted = FALSE AND
    json_typeof(regexp_replace(data, '\\u0000', '', 'g')::JSON->'nodes') = 'object' AND
    json_data.value->'integrationKey' IS NOT NULL AND
    position('$$' in json_data.value->>'integrationKey') <> 0
),
/**
 * Filters out "stale" module versions, i.e., not one of the latest versions
 * or created more than 1 month ago.
 *
 * NOTE: Modify or skip the WHERE clause entirely to adjust the filter condition.
 */
  fresh_active_integrations_in_unpublished AS (
  SELECT
    module_name,
    node_label,
    integration_key,
    created_at
  FROM
    active_integrations_in_unpublished
  WHERE
    version BETWEEN latest_version - 2 AND latest_version OR
    created_at > current_date - interval '1 month'
),
/**
 * Concatenates published and fresh unpublished integration references.
 */
  all_active_integrations AS (
    SELECT * FROM active_integrations_in_published
    UNION ALL
    SELECT * FROM fresh_active_integrations_in_unpublished
),
/**
 * Parses integration info from node data into integration type id and
 * configuration name.
 */
  all_active_integrations_parsed AS (
  SELECT
    module_name,
    node_label,
    created_at,
    split_part(integration_key, '$$', 1) AS type_id_external,
    from_external_id(split_part(integration_key, '$$', 1)) AS type_id,
    split_part(integration_key, '$$', 2) AS config_name
  FROM
    all_active_integrations
),
/**
 * Skips duplicates.
 */
  unique_active_integrations AS (
  SELECT
    min(created_at) AS latest_publish,
    min(type_id_external) AS type_id_external,
    type_id,
    config_name
  FROM
    all_active_integrations_parsed
  GROUP BY
    type_id, config_name
),
/**
 * Fetches info on all configurations.
 */
  configurations AS (
  SELECT
    name,
    created_at,
    integration_type_id AS type_id,
    to_external_id(integration_type_id) AS type_id_external
  FROM
    integration_configurations
)
/**
 * Main query. Outputs configurations in form <type id, config name> of
 * integrations not referenced from any "non-stale" or published module.
 */
SELECT
  configurations.type_id,
  configurations.created_at,
  name AS config_name
FROM
  configurations
LEFT OUTER JOIN
  unique_active_integrations
ON
  configurations.type_id = unique_active_integrations.type_id AND
  configurations.name = unique_active_integrations.config_name
WHERE
  unique_active_integrations.latest_publish is null
;
