from enum import Enum

class Action(Enum):
    LOGIN = 1
    LOGOUT = 2
    MOVE = 3
    TURN = 5
    MAP = 10
    

class Result(Enum):

    OKEY = 0
    BAD_COMMAND = 1
    RESOURCE_NOT_FOUND = 2
    PATH_NOT_FOUND = 3
    ACCESS_DENIED = 5
    
    
class Response(Enum):

    HEADER_ONLY = 7
    FULL = 8
    
   
class ServerData(object):

    HOST  = 'wgforge-srv.wargaming.net'
    PORT = 443
    QUANT = 10  