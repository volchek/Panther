class Train(object):
    
    def __init__(self, json):
        self.speed = json["speed"]
        self.position = json["position"]
        self.line_idx = json["line_idx"]
        self.idx = json["idx"]
      
