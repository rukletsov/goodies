
-- Increase statement timeout for this session only.
BEGIN;
SET statement_timeout = '24h';
COMMIT;


/**
 * Returns all nodes from published and not deleted modules referencing
 * the given integration by <integration type id, config name> passed as
 * `inputs` table in the `WITH` clause.
 *
 * NOTE: Requires `to_external_id()` function.
 *
 * TODO(alexr): Support lookup by configuration id.
 */

WITH inputs (type_id, config_name) AS (
  values ('<uuid here>', '<config name here>')
)
SELECT
  modules.module_id AS module_id,
  published_modules.name AS module_name,
  string_agg(published_modules.module_version::TEXT, ',') AS versions,
  modules.latest_version AS latest_version,
  max(published_modules.created_at) AS most_recent,
  json_data.value->>'label' AS node_label
FROM
  inputs,
  published_modules,
  modules,
  json_each(data::JSON->'nodes') AS json_data
WHERE
  published_modules.module_id = modules.module_id AND
  published_modules.deleted = FALSE AND
  json_typeof(data::JSON->'nodes') = 'object' AND
  json_data.value->'integrationKey' IS NOT NULL AND
  position('$$' in json_data.value->>'integrationKey') <> 0 AND
  json_data.value->>'integrationKey' =
    CONCAT(to_external_id(inputs.type_id::UUID), '$$', inputs.config_name)
GROUP BY modules.module_id, module_name, latest_version, node_label
ORDER BY most_recent DESC
;


/**
 * Returns all nodes from all not yet deleted module versions referencing
 * the given integration by <integration type id, config name> passed as
 * `inputs` table in the `WITH` clause.
 *
 * NOTE: Requires `to_external_id()` function.
 *
 * TODO(alexr): Support lookup by configuration id.
 */
WITH inputs (type_id, config_name) AS (
  values ('<uuid here>', '<config name here>')
)
SELECT
  modules.module_id AS module_id,
  module_versions.name AS module_name,
  string_agg(module_versions.version::TEXT, ',') AS versions,
  modules.latest_version AS latest_version,
  max(module_versions.created_at) AS most_recent,
  json_data.value->>'label' AS node_label
FROM
  inputs,
  module_versions,
  modules,
  json_each(data::JSON->'nodes') AS json_data
WHERE
  module_versions.id = modules.module_id AND
  modules.deleted = FALSE AND
  json_typeof(data::JSON->'nodes') = 'object' AND
  json_data.value->'integrationKey' IS NOT NULL AND
  position('$$' in json_data.value->>'integrationKey') <> 0 AND
  json_data.value->>'integrationKey' =
    CONCAT(to_external_id(inputs.type_id::UUID), '$$', inputs.config_name)
GROUP BY module_id, module_name, latest_version, node_label
ORDER BY most_recent DESC
;
