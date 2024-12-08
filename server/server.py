from flask import Flask, request, jsonify, render_template

app = Flask(__name__)


# Global variable to store the latest sound data
sound_data = []

@app.route("/", methods=["GET", "POST"])
def handle_data():
    if request.method == "POST":
        try:
            # Get the JSON data from the request body
            data = request.get_json()
            sound_level = data.get("soundLevel")

            if sound_level is None:
                return jsonify({"error": "Missing soundLevel in request"}), 400

            # Append sound level to the global list (limit the size to avoid memory issues)
            sound_data.append(sound_level)
            if len(sound_data) > 50:  # Keep only the last 50 sound levels
                sound_data.pop(0)

            return jsonify({"message": "Data received successfully"}), 200
        except Exception as e:
            return jsonify({"error": "Failed to process data", "details": str(e)}), 500

    elif request.method == "GET":
        return jsonify(sound_data), 200


@app.route("/visualize", methods=["GET"])
def visualize():
    return render_template("visualize.html")


# # Global variable to store the latest sound data
# latest_data = {
#     "soundLevel": None,
#     "vibrationCommand": None
# }


# @app.route("/", methods=["GET", "POST"])
# def handle_data():
#     try:
#         if request.method == "POST":
#             # Get the JSON data from the request body
#             data = request.get_json()

#             # Extract sound level from the JSON data
#             sound_level = data.get("soundLevel")

#             if sound_level is None:
#                 return jsonify({"error": "Missing soundLevel in request"}), 400

#             # Process the sound level to generate vibration intensity
#             vibration_command = process_sound_to_vibration(sound_level)

#             # Print the received data to the server logs
#             print(f"Received sound level: {sound_level}")
#             print(f"Generated vibration command: {vibration_command}")

#             # Update the latest data
#             latest_data["soundLevel"] = sound_level
#             latest_data["vibrationCommand"] = vibration_command

#             # Return a success response with vibration command
#             return jsonify({
#                 "message": "Data processed successfully",
#                 "soundLevel": sound_level,
#                 "vibrationCommand": vibration_command
#             }), 200

#         elif request.method == "GET":
#             # Return the latest data
#             return jsonify(latest_data), 200

#         else:
#             return jsonify({"error": "Invalid request method"}), 405

#     except Exception as e:
#         # If an error occurs, send an error response
#         print(f"Error processing request: {e}")  # Log the exception
#         return jsonify({"error": "Failed to process data", "details": str(e)}), 500

# def process_sound_to_vibration(sound_level):
#     """
#     Convert the received sound level into a vibration intensity command.
#     For example, map sound levels (0-255) to vibration strength (0-255).
#     """
#     # For simplicity, return the same value for now. Adjust as needed.
#     return sound_level

