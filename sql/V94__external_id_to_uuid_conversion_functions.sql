
-- Converts given BRYTER `ExternalId`, which is a variant of base64
-- encoding of a uuid, to uuid.
CREATE OR REPLACE FUNCTION from_external_id(text)
  RETURNS UUID AS
$body$
SELECT
  encode(             -- # convert hex to string
    decode(           -- # add '==' at the end and base64 decodes
      replace(        -- # replace '-' with '+'
        replace(      -- # replace '_' with '/'
          $1, '_', '/'
        ), '-', '+'
      ) || '==', 'base64'
    ), 'hex'
  )::UUID
$body$ LANGUAGE SQL;


-- Converts uuid to BRYTER `ExternalId`, which is slightly different
-- from PostgreSQL's base64 encoding.
CREATE OR REPLACE FUNCTION to_external_id(uuid)
  RETURNS TEXT AS
$body$
SELECT
  replace(            -- # replace '/' with '_'
    replace(          -- # replace '+' with '-'
      left(           -- # remove '==' at the end
        encode(       -- # base64 encode
          decode(     -- # convert to hex
            replace(  -- # convert to text and remove all '-'
              $1::TEXT, '-', ''
            ), 'hex'
          ), 'base64'
        ), -2
      ), '+', '-'
    ), '/', '_'
  )
$body$ LANGUAGE SQL;
