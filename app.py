import logging
from flask import Flask, request, jsonify, render_template
from flask_cors import CORS

app = Flask(__name__)
CORS(app)
temperatures = []  # Stores temperature data
humidities = []  # Stores humidity data
soil_moistures = []  # Stores soil moisture data

# Set up logging
logging.basicConfig(filename='app.log', level=logging.DEBUG)

@app.route('/')
def mainpage():
    return render_template('index.html')  # Render a template called index.html

@app.route('/temperature', methods=['POST', 'GET'])
def handle_temperature():
    if request.method == 'POST':
        data = request.json
        print(request.json)  # Add this line to print the received JSON payload
        if data and 'temp' in data and 'humidity' in data and 'soil_moisture' in data:
            temperatures.append(data['temp'])
            humidities.append(data['humidity'])
            soil_moistures.append(data['soil_moisture'])
            logging.info(f"Data received: temp = {data['temp']}, humidity = {data['humidity']}, soil moisture = {data['soil_moisture']}")
            return jsonify({'message': 'Data received'}), 200
        logging.error("Bad request")
        return jsonify({'error': 'Bad request'}), 400
    else:
        # GET request
        last_temp = temperatures[-1] if temperatures else 'No data'
        last_humidity = humidities[-1] if humidities else 'No data'
        last_soil_moisture = soil_moistures[-1] if soil_moistures else 'No data'
        return jsonify({
            'last_temp': last_temp,
            'last_humidity': last_humidity,
            'last_soil_moisture': last_soil_moisture
        })

@app.errorhandler(500)
def handle_500_error(error):
    app.logger.error(f"500 error: {error}")
    return jsonify({'error': 'Internal server error'}), 500

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
