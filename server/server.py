from flask import Flask, request, jsonify, render_template

app = Flask(__name__)


# Global variable to store the latest sound data.
sound_data = []

@app.route("/", methods=["GET", "POST"])
def handle_data():
    if request.method == "POST":
        try:
            # Get the JSON data from the request body.
            data = request.get_json()
            sound_level = data.get("soundLevel")

            if sound_level is None:
                return jsonify({"error": "Missing soundLevel in request"}), 400

            # Append sound level to the global list (limit the size to avoid memory issues).
            sound_data.append(sound_level)
            # Keep only the last 50 sound levels.
            if len(sound_data) > 50:  
                sound_data.pop(0)

            return jsonify({"message": "Data received successfully"}), 200
        except Exception as e:
            return jsonify({"error": "Failed to process data", "details": str(e)}), 500

    elif request.method == "GET":
        return jsonify(sound_data), 200


@app.route("/visualize", methods=["GET"])
def visualize():
    return render_template("visualize.html")
