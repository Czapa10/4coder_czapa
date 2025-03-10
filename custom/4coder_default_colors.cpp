/*
* Default color slots
*/

// TOP

function Color_Array
finalize_color_array(Color_Table table, u64 id){
    return(table.arrays[id % table.count]);
}

function ARGB_Color
finalize_color(Color_Array array, i32 sub_index){
    ARGB_Color result = 0xFFFFFFFF;
    if (array.count > 0){
        result = array.vals[sub_index % array.count];
    }
    return(result);
}

function ARGB_Color
finalize_color(Color_Table color_table, u64 id, i32 sub_index){
    Color_Array array = finalize_color_array(color_table, id);
    return(finalize_color(array, sub_index));
}

function Color_Array
finalize_color_array(u64 id){
	return(finalize_color_array(active_color_table, id));
}

function ARGB_Color
finalize_color(u64 id, i32 sub_index){
	return(finalize_color(active_color_table, id, sub_index));
}

////////////////////////////////

function Color_Array
make_colors(Arena *arena, ARGB_Color color){
    Color_Array result = {};
    result.count = 1;
    result.vals = push_array(arena, ARGB_Color, 1);
    result.vals[0] = color;
    return(result);
}

function Color_Array
make_colors(Arena *arena, ARGB_Color c1, ARGB_Color c2){
    Color_Array result = {};
    result.count = 2;
    result.vals = push_array(arena, ARGB_Color, 2);
    result.vals[0] = c1;
    result.vals[1] = c2;
    return(result);
}

function Color_Array
make_colors(Arena *arena, ARGB_Color c1, ARGB_Color c2, ARGB_Color c3){
    Color_Array result = {};
    result.count = 3;
    result.vals = push_array(arena, ARGB_Color, 3);
    result.vals[0] = c1;
    result.vals[1] = c2;
    result.vals[2] = c3;
    return(result);
}

function Color_Array
make_colors(Arena *arena, ARGB_Color c1, ARGB_Color c2, ARGB_Color c3, ARGB_Color c4){
    Color_Array result = {};
    result.count = 4;
    result.vals = push_array(arena, ARGB_Color, 4);
    result.vals[0] = c1;
    result.vals[1] = c2;
    result.vals[2] = c3;
    result.vals[3] = c4;
    return(result);
}

function Color_Array
make_colors(Arena *arena, ARGB_Color *colors, i32 count){
    Color_Array result = {};
    result.count = count;
    result.vals = push_array_write(arena, ARGB_Color, count, colors);
    return(result);
}

function Color_Table
make_color_table(Application_Links *app, Arena *arena){
    Managed_ID highest_color_id = managed_id_group_highest_id(app, string_u8_litexpr("colors"));
    Color_Table result = {};
    result.count = (u32)(clamp_top(highest_color_id + 1, max_u32));
    result.arrays = push_array(arena, Color_Array, result.count);
    u32 *dummy = push_array(arena, u32, 1);
    *dummy = 0xFF990099;
    for (i32 i = 0; i < result.count; i += 1){
        result.arrays[i].vals = dummy;
        result.arrays[i].count = 1;
    }
    return(result);
}

#include "czapa_color_theme.cpp"

////////////////////////////////

function void
set_active_color(Color_Table *table){
    if (table != 0){
        active_color_table = *table;
    }
}

// TODO(allen): Need to make this nicer.
function void
set_single_active_color(u64 id, ARGB_Color color){
    active_color_table.arrays[id] = make_colors(&global_theme_arena, color);
}

function void
save_theme(Color_Table table, String_Const_u8 name){
    Color_Table_Node *node = push_array(&global_theme_arena, Color_Table_Node, 1);
    sll_queue_push(global_theme_list.first, global_theme_list.last, node);
    global_theme_list.count += 1;
    node->name = push_string_copy(&global_theme_arena, name);
    node->table = table;
}

////////////////////////////////

function Color_Table*
get_color_table_by_name(String_Const_u8 name){
    Color_Table *result = 0;
    for (Color_Table_Node *node = global_theme_list.first;
         node != 0;
         node = node->next){
        if (string_match(node->name, name)){
            result = &node->table;
            break;
        }
    }
    return(result);
}

// BOTTOM
