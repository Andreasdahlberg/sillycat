$(window).load(function() {


var data = [{
  values: [31, 1, 45, 1, 1, 1, 1, 1, 8, 35, 1, 216, 12, 12, 12, 12, 12, 4, 2, 2, 4, 1, 1, 2, 2, 2, 4, 10, 2, 2],
  labels: ["active_config", "current_input", "adc_inputs", "adc_state", "slave_active", "spi_status", "state", "state", "packet_handlers", "packet_frame", "transceiver_state", "node_views", "clock_and_date_view", "clock_view", "sensor_information_view", "temperature_view", "detailed_temperature_view", "auto_timer", "active_view", "root_view", "activity_timer", "refresh_flag", "prev_push", "push_event_callback", "right_event_callback", "left_event_callback", "sample_timer", "sensor_reading", "current_alpha", "current_font"],
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