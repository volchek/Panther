class Path(object):

    def __init__(self, market_ids, to_market_dist_dict, path, length):
        self.market_ids = market_ids
        self.to_market_dist_dict = to_market_dist_dict
        self.path = path
        self.length = length
        self.score = 0
    
    def get_score(self, world, population):
        for k,v in self.to_market_dist_dict.items():
            potential = world.get_potential_market_product(k,v)
            self.score += potential
        self.score -= self.length * population
        return self.score
