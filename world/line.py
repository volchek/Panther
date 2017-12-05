class Line(object):

    def __init__(self, line_data):
        self.start_point_idx = line_data["point"][0]
        self.end_point_idx = line_data["point"][1]
        self.length = line_data["length"]
        self.idx = line_data["idx"]
