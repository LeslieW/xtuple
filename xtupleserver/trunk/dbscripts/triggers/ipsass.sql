CREATE OR REPLACE FUNCTION _ipsassBeforeTrigger () RETURNS TRIGGER AS '
BEGIN

  --  Checks
  IF NOT (checkPrivilege(''MaintainPriceSchedules'')) THEN
    RAISE EXCEPTION ''You do not have privileges to maintain Price Schedules.'';
  END IF;

  -- Business logic, disallow invalid combinations
  IF (TG_OP IN (''INSERT'',''UPDATE'')) THEN
    IF (LENGTH(COALESCE(NEW.ipsass_custtype_pattern,'') != 0)) THEN
      new.ipsass_cust_id 		= 	-1;
      new.ipsass_custttype_id 		= 	-1;
      new.ipsass_shipto_id		= 	-1;
      new.ipsass_shipto_pattern	=	'''';
    ELSIF (COALESCE(NEW.ipsass_custtype_id,-1) > -1) THEN
      new.ipsass_cust_id 		= 	-1;
      new.ipsass_shipto_id		= 	-1;
      new.ipsass_shipto_pattern	=	'''';
      new.ipsass_custtype_pattern	=	'''';
    ELSIF (LENGTH(COALESCE(NEW.ipsass_shipto_pattern,'') != 0)) THEN
      new.ipsass_custttype_id 		= 	-1;
      new.ipsass_shipto_id		= 	-1;
      new.ipsass_custtype_pattern	=	'''';
    ELSE
      new.ipsass_shipto_id		= 	COALESCE(NEW.ipsass_shipto_id,-1);
      new.ipsass_custtype_id 		= 	-1;
      new.ipsass_shipto_pattern	=	'''';
      new.ipsass_custtype_pattern	=	'''';
    END IF;
  END IF;
  
  RETURN NEW;
END;
' LANGUAGE 'plpgsql';

SELECT dropIfExists('TRIGGER', 'ipsassBeforeTrigger');
CREATE TRIGGER ipsassBeforeTrigger BEFORE INSERT OR UPDATE OR DELETE ON ipsass FOR EACH ROW EXECUTE PROCEDURE _ipsassBeforeTrigger();
