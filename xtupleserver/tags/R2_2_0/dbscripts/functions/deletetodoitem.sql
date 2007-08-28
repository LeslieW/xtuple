
CREATE OR REPLACE FUNCTION deleteTodoItem(INTEGER) RETURNS INTEGER AS '
  DECLARE
    ptodoItemId ALIAS FOR $1;
  BEGIN
    DELETE FROM todoitem WHERE todoitem_id = ptodoItemId;
    RETURN 0;
  END;
' LANGUAGE 'plpgsql';

