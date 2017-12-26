#pragma once

namespace Client_settings {
	static const char* default_host = "wgforge-srv.wargaming.net";
	static const char* default_port = "443";
	static const char* default_login_msg = "{\"name\": \"Panthaarrrr\"}";
	static const int buffer_size = 65536;
}

namespace Client_messages {
	static const char* static_map_msg = "{\"layer\": 0}";
	static const char* dynamic_map_msg = "{\"layer\": 1}";
	static const char* coord_map_msg = "{\"layer\": 10}";
	static const char* turn_msg = "{}";
	static const int value_len = 4;
}

namespace Static_map {
	static const char* line_name = "line";
	static const char* map_name = "name";
	static const char* id = "idx";
	static const char* length = "length";
	static const char* points = "point";
	static const int line_count = 180;
	static const int vertice_count = 100;
	static const char* adjacency_name = "point";
	static const char* unit = "post_id";
}

namespace Dynamic_map {
	static const int train_position = 0;
	static const char* train_name = "train";
	static const char* post_name = "post";
	static const char* type = "type";

}

namespace Player_keys {
	static const char* name = "name";
	static const char* idx = "idx"; 
	static const char* home = "home";
	static const char* home_idx = "idx";
	static const char* home_post_id = "post_id";
}


namespace Train_keys {
	static const char* id = "idx";
	static const char* product = "goods";
	static const char* capacity = "goods_capacity";
	static const char* position = "position";
	static const char* speed = "speed";
	static const char* line_index = "line_idx";
	static const char* train_index = "train_idx";
	static const char* player_id = "player_id";
	static const char* level = "level";
	static const char* next_level_price = "next_level_price";
	static const char* post_type = "post_type";
	static const char* cooldown = "cooldown";
	static const int forward = 1;
	static const int back = -1;
}

namespace Town_keys {
	static const char* id = "idx";
	static const char* name = "name";
	static const char* product = "product";
	static const char* product_capacity = "product_capacity";
	static const char* population = "population";
	static const char* population_capacity = "population_capacity";
	static const char* armor = "armor";
	static const char* armor_capacity = "armor_capacity";
	static const char* type = "type";
	static const char* level = "level";
	static const char* price = "next_level_price";
	static const char* point_id = "point_id";
	static const char* cooldown = "train_cooldown_on_collision";
}

namespace Market_keys {
	static const char* id = "idx";
	static const char* name = "name";
	static const char* product = "product";
	static const char* capacity = "product_capacity";
	static const char* regeneration_rate = "replenishment";
	static const char* type = "type";
	static const char* point_id = "point_id";
	static const char* armor = "armor";
	static const char* armor_capacity = "armor_capacity";
}