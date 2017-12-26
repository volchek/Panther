import json
import os
from sys import argv
from time import sleep
from tkinter import Tk, Canvas, Frame, BOTH

# This simple visualizer is based on the tutorial: http://zetcode.com/gui/tkinter/drawing

PATH = "./replay/2017-12-26 06-48-14" # change this path to replay another saved game
TIMEOUT = 0.3
FACTOR = 2
HSHIFT, VSHIFT = 25, 0
USERNAME = "Panthaarrrr"

ROOT = Tk()

class Visualizer(Frame):
    canvas = None

    def __init__(self):
        global canvas
        Frame.__init__(self)
        self.parse_json()
        canvas = Canvas(self)
        self.initUI()
        canvas.update_idletasks()
        ROOT.after(0, self.updateFrames)
    
    def parse_json(self):
        self.vertices = {}
        with open(os.path.join(PATH, "coordinates.txt")) as f:
            data = json.load(f)
        for point in data["coordinate"]:
            self.vertices[point["idx"]] = (FACTOR * point["x"] + HSHIFT, FACTOR * point["y"] + VSHIFT)
        
        self.edges, self.posts = {}, {}
        with open(os.path.join(PATH, "static_map.txt")) as f:
            data = json.load(f)
        for edge in data["line"]:
            self.edges[edge["idx"]] = {"coord": edge["point"], "length": edge["length"]}
        for point in data["point"]:
            if point["post_id"]:
                self.posts[point["idx"]] = point["post_id"]
        
        self.frames = []
        filenames = sorted(set(os.listdir(PATH)) - set(["coordinates.txt", "static_map.txt", "test.txt"]),
                                         key = lambda x : int(x[:-4]))
        for filename in filenames:
            with open(os.path.join(PATH, filename)) as f:
                data = json.load(f)
            frame = {"towns": {}, "markets": {}, "storages": {}, "trains": {}}
            for post in data["post"]:
                if post["type"] == 1:
                    frame["towns"][post["point_id"]] = {"name": post["name"],
                                                        "population": post["population"],
                                                        "product": post["product"],
                                                        "armor": post["armor"]}
                elif post["type"] == 2:
                    frame["markets"][post["point_id"]] = {"name": post["name"].replace("market-", "m"),
                                                          "product": post["product"],
                                                          "product_capacity": post["product_capacity"]}
                elif post["type"] == 3:
                    frame["storages"][post["point_id"]] = {"name": post["name"].replace("storage-", "s"),
                                                           "armor": post["armor"],
                                                           "armor_capacity": post["armor_capacity"]}
            for train in data["train"]:
                frame["trains"][train["idx"]] = {"edge_id": train["line_idx"], "position": train["position"]}
            frame["rating"] = data["rating"][USERNAME]
            self.frames.append(frame)
    
    def initUI(self):
        self.master.title("Simple WF Visualizer")
        self.pack(fill=BOTH, expand=1)
        
        for edge_idx in self.edges:
            idx1, idx2 = self.edges[edge_idx]["coord"]
            x1, y1 = self.vertices[idx1]
            x2, y2 = self.vertices[idx2]
            canvas.create_line(x1, y1, x2, y2, fill="#999")
        canvas.pack(fill=BOTH, expand=1)
    
    def updateFrames(self):
        for frame in self.frames:
            canvas.delete("all")
            self.initUI()
            canvas.update_idletasks()
            for town in frame["towns"]:
                x, y = self.vertices[town]
                canvas.create_oval(x-4, y-4, x+4, y+4, outline="#F00", fill="#F00")
                canvas.create_text(x, y+4, anchor="n", text=frame["towns"][town]["name"])
                canvas.create_text(x, y-4, anchor="s",
                                   text="%s|%s|%s" % (frame["towns"][town]["population"],
                                                      frame["towns"][town]["product"],
                                                      frame["towns"][town]["armor"]))
            for market in frame["markets"]:
                x, y = self.vertices[market]
                canvas.create_oval(x-3, y-3, x+3, y+3, outline="#0F0", fill="#0F0")
                canvas.create_text(x, y+3, anchor="n", text=frame["markets"][market]["name"])
                canvas.create_text(x, y-3, anchor="s",
                                   text="%s/%s" % (frame["markets"][market]["product"], frame["markets"][market]["product_capacity"]))
            for storage in frame["storages"]:
                x, y = self.vertices[storage]
                canvas.create_oval(x-3, y-3, x+3, y+3, outline="#00F", fill="#00F")
                canvas.create_text(x, y+3, anchor="n", text=frame["storages"][storage]["name"])
                canvas.create_text(x, y-3, anchor="s",
                                   text="%s/%s" % (frame["storages"][storage]["armor"], frame["storages"][storage]["armor_capacity"]))
            for train in frame["trains"]:
                edge_id, position = frame["trains"][train]["edge_id"], frame["trains"][train]["position"]
                x_start, y_start = self.vertices[self.edges[edge_id]["coord"][0]]
                x_end, y_end = self.vertices[self.edges[edge_id]["coord"][1]]
                ratio = float(position) / self.edges[edge_id]["length"]
                x_shift = ratio * (x_end - x_start)
                y_shift = ratio * (y_end - y_start)
                x = x_start + x_shift
                y = y_start + y_shift
                canvas.create_rectangle(x-2, y-2, x+2, y+2, outline="#000", fill="#000")
            canvas.create_text(225, 415, anchor="n", text="Rating: %s" % frame["rating"])
            canvas.update_idletasks()
            sleep(TIMEOUT)

def main():
    vis = Visualizer()
    ROOT.geometry("450x450+100+100")
    ROOT.mainloop()

if __name__ == '__main__':
    main()