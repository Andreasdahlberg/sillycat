$(window).load(function() {

var data = [{
  values: $values$,
  labels: $labels$,
            'name': 'Variables',
                    'domain': {'x': [0, 0.48]},
                    'hoverinfo':'label+percent',
                    'textinfo': 'none',                  
                    'hole': 0.2,  
  type: 'pie'
}];

var layout = {
  'title': 'Memory usage',
  'showlegend': true,
  'autosize': true
};

Plotly.newPlot('tester', data, layout, {'displayModeBar': false});
  $('#loader').fadeOut(300, function () {
    $('#tester').fadeIn(300);
  });
});