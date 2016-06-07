$(window).load(function() {

var data = [{
  values: [12, 31, 1, 45, 1, 4, 1, 1, 2, 2, 2, 8, 35, 1, 1, 1, 72, 12, 12, 12, 12, 12, 4, 2, 2, 4, 1, 1, 2, 2, 2, 4, 10, 2, 12, 105, 1, 512, 2, 1100],
  labels: ["days_in_months", "active_config", "current_input", "adc_inputs", "adc_state", "reset_time_ms", "slave_active", "spi_status", "tx_handler", "rx_handler", "state", "packet_handlers", "packet_frame", "transceiver_state", "current_id", "current_index", "node_views", "clock_and_date_view", "clock_view", "sensor_information_view", "temperature_view", "detailed_temperature_view", "auto_timer", "active_view", "root_view", "activity_timer", "refresh_flag", "prev_push", "push_event_callback", "right_event_callback", "left_event_callback", "sample_timer", "sensor_reading", "current_alpha", "last_update", "packets", "dislay_rotated", "VRAM", "current_font", "free"],
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