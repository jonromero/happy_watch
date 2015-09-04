Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready!');
});

Pebble.addEventListener('showConfiguration', function() {
  var url = 'https://dl.dropboxusercontent.com/u/757245/happy%20watch/config/index.html';
  console.log('Showing configuration page: ' + url);

  Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
  var configData = JSON.parse(decodeURIComponent(e.response));
  console.log('Configuration page returned: ' + JSON.stringify(configData));

  var backgroundColor = configData['background_color'];
  var textColor = configData['text_color'];
  
  var dict = {};
  dict['KEY_COLOR_RED'] = parseInt(backgroundColor.substring(2, 4), 16);
  dict['KEY_COLOR_GREEN'] = parseInt(backgroundColor.substring(4, 6), 16);
  dict['KEY_COLOR_BLUE'] = parseInt(backgroundColor.substring(6), 16);
  
  dict['KEY_COLOR_RED_T'] = parseInt(textColor.substring(2, 4), 16);
  dict['KEY_COLOR_GREEN_T'] = parseInt(textColor.substring(4, 6), 16);
  dict['KEY_COLOR_BLUE_T'] = parseInt(textColor.substring(6), 16);
  
  // Send to watchapp
  Pebble.sendAppMessage(dict, function() {
    console.log('Send successful: ' + JSON.stringify(dict));
  }, function() {
    console.log('Send failed!');
  });
});
