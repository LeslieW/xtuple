REVOKE ALL ON obsolete_apchk_apchk_id_seq FROM PUBLIC;
REVOKE ALL ON obsolete_apchkitem_apchkitem_id_seq FROM PUBLIC;
REVOKE ALL ON printjob FROM PUBLIC;
REVOKE ALL ON printjob_printjob_id_seq FROM PUBLIC;
REVOKE ALL ON printjobparam FROM PUBLIC;
REVOKE ALL ON printjobparam_printjobparam_id_seq FROM PUBLIC;
REVOKE ALL ON backup_ccpay FROM PUBLIC;
REVOKE ALL ON backup_payco FROM PUBLIC;
GRANT ALL ON obsolete_apchk_apchk_id_seq TO GROUP openmfg;
GRANT ALL ON obsolete_apchkitem_apchkitem_id_seq TO GROUP openmfg;
GRANT ALL ON printjob TO GROUP openmfg;
GRANT ALL ON printjob_printjob_id_seq TO GROUP openmfg;
GRANT ALL ON printjobparam TO GROUP openmfg;
GRANT ALL ON printjobparam_printjobparam_id_seq TO GROUP openmfg;
GRANT ALL ON backup_ccpay TO GROUP openmfg;
GRANT ALL ON backup_payco TO GROUP openmfg;
