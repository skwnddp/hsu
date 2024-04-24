function onEdit(e) {
  var editedRange = e.range;
  var editedSheet = editedRange.getSheet();
  var editedRow = editedRange.getRow();
  var editedColumn = editedRange.getColumn();
  
  var targetSheet = editedSheet;
  var targetRange = targetSheet.getRange("C3:C");
  var targetData = targetRange.getValues();
  var targetColumn = 3; // Column C
  
  if (editedColumn === targetColumn && editedRow >= 3) {
    var rowIndex = editedRow - 3;
    var targetCell = targetRange.getCell(rowIndex + 1, 1);
    
    var currentDate = new Date();
    var outputSheet = targetSheet;
    var outputRange = outputSheet.getRange("D3:D");
    outputRange.getCell(rowIndex + 1, 1).setValue(currentDate);
  }
}
