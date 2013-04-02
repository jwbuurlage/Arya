
--
-- This script declares new unit types
-- A unit type is uniquely identified by a number
-- which is also the number that the server sends the client
-- to specify what unit it is
--
-- The unit scripts, loaded below, will use the global variable
-- called newUnitTypeId for this number.
--

newUnitTypeId = 0;
require("warrior")

newUnitTypeId = 1;
require("ranger")

newUnitTypeId = 2;
require("imp")

newUnitTypeId = 3;
require("house")

newUnitTypeId = 4;
require("chapel")

newUnitTypeId = 5;
require("mill")

