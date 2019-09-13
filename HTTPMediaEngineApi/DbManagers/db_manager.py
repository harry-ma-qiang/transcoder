from abc import ABC, abstractmethod


class DBManager(ABC):
    @abstractmethod
    def add_signal(self, signal):
        pass

    @abstractmethod
    def update_signal(self, signal):
        pass

    @abstractmethod
    def delete_signal(self, signalId):
        pass

    # @abstractmethod
    # def add_video_wall(self, wall):
    #     pass
    #
    # @abstractmethod
    # def update_video_wall(self, wallId):
    #     pass
    #
    # @abstractmethod()
    # def delete_video_wall(self, wallId):
    #     pass

    # @abstractmethod
    # def add_task(self, task):
    #     pass
    #
    # @abstractmethod
    # def delete_task(self, taskId):
    #     pass
    #
    # @abstractmethod
    # def get_task_details(self, taskId):
    #     pass
