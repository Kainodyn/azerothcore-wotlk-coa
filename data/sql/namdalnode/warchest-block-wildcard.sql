-- NamdalNode: block the hardcoded level-10 Elite Warchest (item 2977351,
-- src/server/coa/AscensionWelcomeWarchest.cpp) for every account.
-- The core skips accounts that already have a row in coa_account_warchest,
-- so this marks all existing accounts and every new one as "claimed".
--
-- NOT applied by the DB updater (this folder is not in updates_include).
-- Run by hand as MySQL root (binlog is on, CREATE TRIGGER needs SUPER):
--   sudo mysql < data/sql/namdalnode/warchest-block-wildcard.sql
-- Idempotent. Needs acore_auth, acore_characters and acore_characters_wc.
-- Variant for two realms: also covers acore_characters_wc (both DBs must exist).
-- Undo: DROP TRIGGER acore_auth.coa_block_warchest_new_account;

INSERT IGNORE INTO acore_characters.coa_account_warchest (account, claimed_at)
SELECT id, UNIX_TIMESTAMP() FROM acore_auth.account;
INSERT IGNORE INTO acore_characters_wc.coa_account_warchest (account, claimed_at)
SELECT id, UNIX_TIMESTAMP() FROM acore_auth.account;

DROP TRIGGER IF EXISTS acore_auth.coa_block_warchest_new_account;

DELIMITER $$
CREATE DEFINER=`acore`@`127.0.0.1` TRIGGER acore_auth.coa_block_warchest_new_account
AFTER INSERT ON acore_auth.account FOR EACH ROW
BEGIN
  INSERT IGNORE INTO acore_characters.coa_account_warchest (account, claimed_at) VALUES (NEW.id, UNIX_TIMESTAMP());
  INSERT IGNORE INTO acore_characters_wc.coa_account_warchest (account, claimed_at) VALUES (NEW.id, UNIX_TIMESTAMP());
END$$
DELIMITER ;
