
    // NOTE: My custom default color theme
function void
set_default_color_scheme(Application_Links *app){
    if (global_theme_arena.base_allocator == 0){
        global_theme_arena = make_arena_system();
    }
    
    Arena *arena = &global_theme_arena;
    
    default_color_table = make_color_table(app, arena);
    
    ARGB_Color Margin = 0xFF262626;
    ARGB_Color MarginHover = 0xFF333333;
    ARGB_Color MarginActive = 0xFF404040;
    ARGB_Color Back = 0xFF161616;
    ARGB_Color Value = 0xFF6B8E23;
    
    default_color_table.arrays[0] = make_colors(arena, 0xFF90B080);
    default_color_table.arrays[defcolor_bar] = make_colors(arena, 0xFF888888);
    default_color_table.arrays[defcolor_base] = make_colors(arena, 0xFF000000);
    default_color_table.arrays[defcolor_pop1] = make_colors(arena, 0xFF3C57DC);
    default_color_table.arrays[defcolor_pop2] = make_colors(arena, 0xFFFF0000);
    default_color_table.arrays[defcolor_back] = make_colors(arena, Back);
    default_color_table.arrays[defcolor_margin] = make_colors(arena, Margin);
    default_color_table.arrays[defcolor_margin_hover] = make_colors(arena, MarginHover);
    default_color_table.arrays[defcolor_margin_active] = make_colors(arena, MarginActive);
    default_color_table.arrays[defcolor_list_item] = make_colors(arena, Margin, Back);
    default_color_table.arrays[defcolor_list_item_hover] = make_colors(arena, MarginHover, Margin);
    default_color_table.arrays[defcolor_list_item_active] = make_colors(arena, MarginActive, MarginActive);
    default_color_table.arrays[defcolor_cursor] = make_colors(arena, 0xFF40FF40, 0xFFFF4040);
    default_color_table.arrays[defcolor_at_cursor] = make_colors(arena, Back);
    default_color_table.arrays[defcolor_highlight_cursor_line] = make_colors(arena, 0xFF121E12);
    default_color_table.arrays[defcolor_highlight] = make_colors(arena, 0xFF703419);
    default_color_table.arrays[defcolor_at_highlight] = make_colors(arena, 0xFFCDAA7D);
    default_color_table.arrays[defcolor_mark] = make_colors(arena, 0xFF808080);
    default_color_table.arrays[defcolor_text_default] = make_colors(arena, 0xFFA08563);
    default_color_table.arrays[defcolor_comment] = make_colors(arena, 0xFF7D7D7D);
    default_color_table.arrays[defcolor_comment_pop] = make_colors(arena, 0xFF00A000, 0xFFA00000);
    default_color_table.arrays[defcolor_keyword] = make_colors(arena, 0xFFCD950C);
    default_color_table.arrays[defcolor_str_constant] = make_colors(arena, Value);
    default_color_table.arrays[defcolor_char_constant] = make_colors(arena, Value);
    default_color_table.arrays[defcolor_int_constant] = make_colors(arena, Value);
    default_color_table.arrays[defcolor_float_constant] = make_colors(arena, Value);
    default_color_table.arrays[defcolor_bool_constant] = make_colors(arena, Value);
    default_color_table.arrays[defcolor_preproc] = make_colors(arena, 0xFFDAB98F);
    default_color_table.arrays[defcolor_include] = make_colors(arena, Value);
    default_color_table.arrays[defcolor_special_character] = make_colors(arena, 0xFFFF0000);
    default_color_table.arrays[defcolor_ghost_character] = make_colors(arena, 0xFF5B4D3C);
    default_color_table.arrays[defcolor_highlight_junk] = make_colors(arena, 0xFF3A0000);
    default_color_table.arrays[defcolor_highlight_white] = make_colors(arena, 0xFF003A3A);
    default_color_table.arrays[defcolor_paste] = make_colors(arena, 0xFFFFBB00);
    default_color_table.arrays[defcolor_undo] = make_colors(arena, 0xFF80005D);
    default_color_table.arrays[defcolor_back_cycle] = make_colors(arena, 0x0CA00000, 0x0800A000, 0x080000A0, 0x08A0A000);
    default_color_table.arrays[defcolor_text_cycle] = make_colors(arena, 0xFFA00000, 0xFF00A000, 0xFF0020B0, 0xFFA0A000);
    default_color_table.arrays[defcolor_line_numbers_back] = make_colors(arena, 0xFF202020);
    default_color_table.arrays[defcolor_line_numbers_text] = make_colors(arena, 0xFF484848);
    
    active_color_table = default_color_table;
}

// NOTE: Original default color theme
/* 
function void
set_default_color_scheme(Application_Links *app){
    if (global_theme_arena.base_allocator == 0){
        global_theme_arena = make_arena_system();
    }
    
    Arena *arena = &global_theme_arena;
    
    default_color_table = make_color_table(app, arena);
    
    default_color_table.arrays[0] = make_colors(arena, 0xFF90B080);
    default_color_table.arrays[defcolor_bar] = make_colors(arena, 0xFF888888);
    default_color_table.arrays[defcolor_base] = make_colors(arena, 0xFF000000);
    default_color_table.arrays[defcolor_pop1] = make_colors(arena, 0xFF3C57DC);
    default_color_table.arrays[defcolor_pop2] = make_colors(arena, 0xFFFF0000);
    default_color_table.arrays[defcolor_back] = make_colors(arena, 0xFF0C0C0C);
    default_color_table.arrays[defcolor_margin] = make_colors(arena, 0xFF181818);
    default_color_table.arrays[defcolor_margin_hover] = make_colors(arena, 0xFF252525);
    default_color_table.arrays[defcolor_margin_active] = make_colors(arena, 0xFF323232);
    default_color_table.arrays[defcolor_list_item] = make_colors(arena, 0xFF181818, 0xFF0C0C0C);
    default_color_table.arrays[defcolor_list_item_hover] = make_colors(arena, 0xFF252525, 0xFF181818);
    default_color_table.arrays[defcolor_list_item_active] = make_colors(arena, 0xFF323232, 0xFF323232);
    default_color_table.arrays[defcolor_cursor] = make_colors(arena, 0xFF00EE00, 0xFFEE7700);
    default_color_table.arrays[defcolor_at_cursor] = make_colors(arena, 0xFF0C0C0C);
    default_color_table.arrays[defcolor_highlight_cursor_line] = make_colors(arena, 0xFF1E1E1E);
    default_color_table.arrays[defcolor_highlight] = make_colors(arena, 0xFFDDEE00);
    default_color_table.arrays[defcolor_at_highlight] = make_colors(arena, 0xFFFF44DD);
    default_color_table.arrays[defcolor_mark] = make_colors(arena, 0xFF494949);
    default_color_table.arrays[defcolor_text_default] = make_colors(arena, 0xFF90B080);
    default_color_table.arrays[defcolor_comment] = make_colors(arena, 0xFF2090F0);
    default_color_table.arrays[defcolor_comment_pop] = make_colors(arena, 0xFF00A000, 0xFFA00000);
    default_color_table.arrays[defcolor_keyword] = make_colors(arena, 0xFFD08F20);
    default_color_table.arrays[defcolor_str_constant] = make_colors(arena, 0xFF50FF30);
    default_color_table.arrays[defcolor_char_constant] = make_colors(arena, 0xFF50FF30);
    default_color_table.arrays[defcolor_int_constant] = make_colors(arena, 0xFF50FF30);
    default_color_table.arrays[defcolor_float_constant] = make_colors(arena, 0xFF50FF30);
    default_color_table.arrays[defcolor_bool_constant] = make_colors(arena, 0xFF50FF30);
    default_color_table.arrays[defcolor_preproc] = make_colors(arena, 0xFFA0B8A0);
    default_color_table.arrays[defcolor_include] = make_colors(arena, 0xFF50FF30);
    default_color_table.arrays[defcolor_special_character] = make_colors(arena, 0xFFFF0000);
    default_color_table.arrays[defcolor_ghost_character] = make_colors(arena, 0xFF4E5E46);
    default_color_table.arrays[defcolor_highlight_junk] = make_colors(arena, 0xFF3A0000);
    default_color_table.arrays[defcolor_highlight_white] = make_colors(arena, 0xFF003A3A);
    default_color_table.arrays[defcolor_paste] = make_colors(arena, 0xFFDDEE00);
    default_color_table.arrays[defcolor_undo] = make_colors(arena, 0xFF00DDEE);
    default_color_table.arrays[defcolor_back_cycle] = make_colors(arena, 0xFF130707, 0xFF071307, 0xFF070713, 0xFF131307);
    default_color_table.arrays[defcolor_text_cycle] = make_colors(arena, 0xFFA00000, 0xFF00A000, 0xFF0030B0, 0xFFA0A000);
    default_color_table.arrays[defcolor_line_numbers_back] = make_colors(arena, 0xFF101010);
    default_color_table.arrays[defcolor_line_numbers_text] = make_colors(arena, 0xFF404040);
    
    active_color_table = default_color_table;
}
 */
