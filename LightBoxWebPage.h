
String light_box_html = R"=====(
  <html>
    <head>
      <link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'>
      <title>Light Box</title>
    </head>
  <style> input { width: 80%; } </style>
  <body>
  <div class='w3-container'><h1>Welcome to Light Box</h1></div> 
  <form id='color_form' name='color_form' method='post' action='/mode' class='w3-container w3-border w3-margin'> 
    <table class='w3-table'>
      <tr><td colspan='2'><h><center>Front</center></h></td> <td colspan='2'><h><center>Back</center></h></td></tr>
      <tr><td class='w3-right-align'>Red</td><td> <input type='range' min='0' max='250' value='80' id='red' name='f_red'></td>
          <td class='w3-right-align'>Red</td><td> <input type='range' min='0' max='250' value='80' id='red' name='b_red'></td></tr>
      <tr><td class='w3-right-align'>Green</td><td> <input type='range' min='0' max='250' value='80' id='green' name='f_green'></td>
          <td class='w3-right-align'>Green</td><td> <input type='range' min='0' max='250' value='80' id='green' name='b_green'></td></tr>
      <tr><td class='w3-right-align'>Blue</td><td> <input type='range' min='0' max='250' value='80' id='blue' name='f_blue'></td>
          <td class='w3-right-align'>Blue</td><td> <input type='range' min='0' max='250' value='80' id='blue' name='b_blue'></td></tr>
      <tr><td class='w3-right-align'>Pulse</td> <td><input type='range' min='0' max='10' value='5' id='pulse_step' name='f_pulse_step'></td>
          <td class='w3-right-align'>Pulse</td> <td><input type='range' min='0' max='10' value='5' id='pulse_step' name='b_pulse_step'></td></tr>
      <tr><td colspan='4'><center><input type='submit' value='update' class='w3-button w3-gray w3-round-xlarge w3-round-xlarge' style='width:120px'></td></tr>
    </table>
  </form>
  <script>
    function formSubmit(event) {
      event.preventDefault(); // Stop normal submissio
    
      var url = '/json';
      var request = new XMLHttpRequest();
      request.open('POST', url, true);
      request.setRequestHeader('Content-Type', 'application/json;charset=UTF-8');
    
      let form_data = new FormData(event.target);
      let form_obj = Object.fromEntries(form_data.entries());
      let f_pulse_step = parseInt(form_obj['f_pulse_step'], 10);
      let b_pulse_step = parseInt(form_obj['b_pulse_step'], 10);
      let front_data = {'red':parseInt(form_obj['f_red'], 10),
                        'green':parseInt(form_obj['f_green'], 10),
                        'blue':parseInt(form_obj['f_blue'], 10),
                        'pulse_step':f_pulse_step };
      let back_data = {'red':parseInt(form_obj['b_red'], 10),
                        'green':parseInt(form_obj['b_green'], 10),
                        'blue':parseInt(form_obj['b_blue'], 10),
                        'pulse_step':b_pulse_step };
      let data = {'front_data':front_data, 'back_data':back_data};
      const data_text = JSON.stringify(data, null, 2);
      console.log(data_text);
      request.send(data_text);
    }
    document.getElementById('color_form').addEventListener('submit', formSubmit);
  </script>
  </body>
  </html>
  )=====";