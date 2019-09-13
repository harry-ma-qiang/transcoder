# Import the framework
from flask import Flask, jsonify, request
from flask_classful import FlaskView
from Views import signal, wall, transcoding, license
from DbManagers.filesystem_db_manager import FileSystemDBManager

app = Flask(__name__)

# set DB type and configuration
path_config = {
    "signal": "/home/yarik/PycharmProjects/HTTPMediaEngineApi/Storage/Signal/"
}
db = FileSystemDBManager(path_config)

# set routing system
signal.SignalView.register(app, init_argument=db)
wall.WallView.register(app, init_argument=db)
transcoding.TaskView.register(app, init_argument=db)
license.LicenseView.register(app, init_argument=db)

if __name__ == '__main__':
    app.run(debug=True)