from DbManagers.db_manager import DBManager
import json
import os.path


class FileSystemDBManager(DBManager):
    def __init__(self, path_config):
        self.path_config = path_config


    def add_signal(self, signal):
        file_path = str(self.path_config.get('signal')) + str(signal.get('signalId')) + ".json"

        if os.path.exists(file_path):
            raise Exception('The signal with id {} already exists.'.format(signal.get('signalId')))

        file = open(file_path, "w")
        file.write(json.dumps(signal))
        file.close()

    def update_signal(self, signal):
        file_path = str(self.path_config.get('signal')) + str(signal.get('signalId')) + ".json"

        if not os.path.exists(file_path):
            raise Exception('The signal with id {} doesn\'t exists.'.format(signal.get('signalId')))

        file = open(file_path, "w")
        file.write(json.dumps(signal))
        file.close()

    def delete_signal(self, signalId):
        file_path = str(self.path_config.get('signal')) + signalId + ".json"

        if not os.path.exists(file_path):
            raise Exception('The signal with id {} doesn\'t exists.'.format(signalId))

        os.remove(file_path)
