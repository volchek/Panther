import networkx as nx
import json
from client import Client

class Runner(object):
    def __init__(self, name):
        self.client = Client(name)
        
    def run(self):
        print("LOGIN:")
        code, answer = self.client.login()
        print code, answer
        print("\nSTATIC_MAP:")
        code, answer = self.client.get_static_map()
        print code, answer
        self.client.play()
        print("\nLOGOUT")    
        code, answer = self.client.logout()
        print code, answer
        self.client.close_socket()

    
if __name__ == "__main__":   
    Runner("Panther").run()    