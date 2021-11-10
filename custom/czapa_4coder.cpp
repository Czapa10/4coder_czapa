#include <cstdint>
#include <cstring>

#define rstd_Implementation
#define rstd_Debug 1
#define rstd_size uint32_t
#include "rstd.h"

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

#define null nullptr
#define global static
#define fn static auto
#define template_fn template<class type> fn

#define ForCount(_Count) for(u32 Index = 0; Index < Count; ++Index)

#if !defined(FAODER_DEFAULT_BINDINGS_APP)
#define FAODER_DEFAULT_BINDINGS_APP

#include "czapa_default_include.cpp"

#if !defined(META_PASS)
#include "generated/managed_id_metadata.cpp"
#endif

String_ID InsertModeMapId;
String_ID NormalModeMapId;
String_ID DeleteModeMapId;
String_ID ChangeModeMapId;
String_ID PanelModeMapId;

bool OpNumberModified;
u32 OpNumber = 1;

fn SetCurrentMapId
(Application_Links* App, Command_Map_ID MapId)
{
    View_ID View = get_active_view(App, 0);
    Buffer_ID Buffer = view_get_buffer(App, View, 0);
    Managed_Scope Scope = buffer_get_managed_scope(App, Buffer);
    Command_Map_ID* MapIdPtr = scope_attachment(App, Scope, buffer_map_id, Command_Map_ID);
    *MapIdPtr = MapId;
}

fn GetColorInt
(u8 R, u8 G, u8 B)
{
    union color
    {
        struct
        { u8 B, G, R, A; };
        
        u32 Int;
    };
    
    color C;
    C.R = R;
    C.G = G;
    C.B = B;
    C.A = 255;
    return C.Int;
}

const u32 White = GetColorInt(250, 250, 250);
const u32 Black = GetColorInt(0, 0, 0);

fn ToStringConstU8
(const char* Str)
{
	String_Const_u8 Res;
	Res.str = (u8*)Str;
	Res.size = strlen(Str);
	return Res;
}

fn WriteText(Application_Links* App, const char* Str)
{ write_text(App, ToStringConstU8(Str)); }

CUSTOM_COMMAND_SIG(WriteTodo)
{ WriteText(app, " // TODO: "); }

CUSTOM_COMMAND_SIG(WriteTodoNow)
{ WriteText(app, " // TODO(now): "); }

CUSTOM_COMMAND_SIG(WriteNote)
{ WriteText(app, " // NOTE: "); }

fn SetNormalModeColors()
{
    u32 Orange = GetColorInt(230, 100, 20);
    active_color_table.arrays[defcolor_cursor].vals[0] = Orange;
    active_color_table.arrays[defcolor_at_cursor].vals[0] = White;
    active_color_table.arrays[defcolor_margin_active].vals[0] = Orange;
}

fn ResetOpNumberState()
{
    OpNumber = 1;
    OpNumberModified = false;
}

CUSTOM_COMMAND_SIG(GoToNormalMode)
{
    SetCurrentMapId(app, NormalModeMapId);
	SetNormalModeColors();
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(GoToInsertMode)
{
    SetCurrentMapId(app, InsertModeMapId);
    
    u32 Green = GetColorInt(50, 230, 50);
    active_color_table.arrays[defcolor_cursor].vals[ 0 ] = Green;
    active_color_table.arrays[defcolor_at_cursor].vals[ 0 ] = White;
    active_color_table.arrays[defcolor_margin_active].vals[ 0 ] = Green;
}

CUSTOM_COMMAND_SIG(GoToDeleteMode)
{
    SetCurrentMapId(app, DeleteModeMapId);
    
    u32 Red = GetColorInt(230, 50, 50);
    active_color_table.arrays[defcolor_cursor].vals[0] = Red;
    active_color_table.arrays[defcolor_at_cursor].vals[0] = White;
    
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(GoToChangeMode)
{
    SetCurrentMapId(app, ChangeModeMapId);
    
    u32 Yellow = GetColorInt(200, 200, 50);
    active_color_table.arrays[defcolor_cursor].vals[0] = Yellow;
    active_color_table.arrays[defcolor_at_cursor].vals[0] = Black;
    
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(GoToPanelMode)
{
    SetCurrentMapId(app, PanelModeMapId);
    
    u32 Pink = GetColorInt(255, 105, 180);
    active_color_table.arrays[defcolor_cursor].vals[0] = Pink;
    active_color_table.arrays[defcolor_at_cursor].vals[0] = White;
}

CUSTOM_COMMAND_SIG(MoveUp)
{
    for(u32 I = 0; I < OpNumber; ++I)
        move_up(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveDown)
{
    for(u32 I = 0; I < OpNumber; ++I)
        move_down(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveRight)
{
    for(u32 I = 0; I < OpNumber; ++I)
        move_right(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveLeft)
{
    for(u32 I = 0; I < OpNumber; ++I)
        move_left(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveAlphaNumbericBoundaryLeft)
{
    for(u32 I = 0; I < OpNumber; ++I)
        move_left_alpha_numeric_boundary(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveAlphaNumbericOrCamelBoundaryLeft)
{
    for(u32 I = 0; I < OpNumber; ++I)
        move_left_alpha_numeric_or_camel_boundary(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveAlphaNumbericBoundaryRight)
{
    for(u32 I = 0; I < OpNumber; ++I)
        move_right_alpha_numeric_boundary(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveAlphaNumbericOrCamelBoundaryRight)
{
    for(u32 I = 0; I < OpNumber; ++I)
        move_right_alpha_numeric_or_camel_boundary(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveLineUp)
{
    for(u32 I = 0; I < OpNumber; ++I)
        move_line_up(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveLineDown)
{
    for(u32 I = 0; I < OpNumber; ++I)
        move_line_down(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveUpToBlankLineEnd)
{
    for(u32 I = 0; I < OpNumber; ++I)
        move_up_to_blank_line_end(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(MoveDownToBlankLineEnd)
{
    for(u32 I = 0; I < OpNumber; ++I)
        move_down_to_blank_line_end(app);
    ResetOpNumberState();
}

CUSTOM_COMMAND_SIG(ChangeActivePanel)
{
	change_active_panel(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(ChangeActivePanelBackwards)
{
	change_active_panel_backwards(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(SwapPanels)
{
    swap_panels(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(ViewBufferOtherPanel)
{
	view_buffer_other_panel(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(OpenPanelVSplit)
{
	open_panel_vsplit(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(OpenPanelHSplit)
{
	open_panel_hsplit(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(ClosePanel)
{
	close_panel(app);
    GoToNormalMode(app);
}

fn BeginUndoRedoBarrier
(Application_Links* app)
{
    View_ID ViewId = get_active_view(app, Access_Always);
    Buffer_ID Buffer = view_get_buffer(app, ViewId, Access_Always);
    History_Group HistoryGroup = history_group_begin(app, Buffer);
    return HistoryGroup;
}

#define ScopeUndoRedoBarrier \
auto _HisGroup = BeginUndoRedoBarrier(app); \
rstd_defer(history_group_end(_HisGroup));

CUSTOM_COMMAND_SIG(ModalDeleteWordRight)
{
    ScopeUndoRedoBarrier;
    for(u32 I = 0; I < OpNumber; ++I)
        delete_alpha_numeric_boundary(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(ModalDeleteWordLeft)
{
    ScopeUndoRedoBarrier;
    for(u32 I = 0; I < OpNumber; ++I)
        backspace_alpha_numeric_boundary(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(ModalDeleteLine)
{
    ScopeUndoRedoBarrier;
    delete_line(app);
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(ModalDeleteUp)
{
    ScopeUndoRedoBarrier;
    delete_line(app);
    for(u32 I = 0; I < OpNumber; ++I)
    {
        move_up(app);
        delete_line(app);
    }
    GoToNormalMode(app);
}

CUSTOM_COMMAND_SIG(ModalDeleteDown)
{
    ScopeUndoRedoBarrier;
    delete_line(app);
    for(u32 I = 0; I < OpNumber; ++I)
        delete_line(app);
    GoToNormalMode(app);
}

const char EnterString[2] = {10, 0};
CUSTOM_COMMAND_SIG(ModalChangeLine)
{
    ScopeUndoRedoBarrier;
    delete_line(app);
	WriteText(app, EnterString);
    GoToInsertMode(app);
	move_up(app);
}

CUSTOM_COMMAND_SIG(ModalChangeWordRight)
{
    ScopeUndoRedoBarrier;
    for(u32 I = 0; I < OpNumber; ++I)
        delete_alpha_numeric_boundary(app);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(ModalChangeWordLeft)
{
    ScopeUndoRedoBarrier;
    for(u32 I = 0; I < OpNumber; ++I)
        backspace_alpha_numeric_boundary(app);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(ModalChangeUp)
{
    ScopeUndoRedoBarrier;
    delete_line(app);
    for(u32 I = 0; I < OpNumber; ++I)
    {
        move_up(app);
        delete_line(app);
    }
	WriteText(app, EnterString);
	move_up(app);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(ModalChangeDown)
{
    ScopeUndoRedoBarrier;
    delete_line(app);
    for(u32 I = 0; I < OpNumber; ++I)
        delete_line(app);
	WriteText(app, EnterString);
	move_up(app);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(NewLine)
{
	seek_end_of_line(app);
	WriteText(app, EnterString);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(NewLine2)
{
	char TwoEntersString[3] = {10, 10, 0};
	seek_end_of_line(app);
	WriteText(app, TwoEntersString);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(NewLineStayingAtTheSamePlace)
{
	seek_end_of_line(app);
	WriteText(app, EnterString);
    move_up(app);
}

CUSTOM_COMMAND_SIG(MoveRightAndGoToInsertMode)
{
    move_right(app);
    GoToInsertMode(app);
}

CUSTOM_COMMAND_SIG(SelectLine)
{
    seek_beginning_of_line(app);
    set_mark(app);
    seek_end_of_line(app);
}

fn InputNumber
(u32 N)
{
    if(OpNumberModified)
    {
        OpNumber *= 10;
        OpNumber += N;
    }
    else if(N != 0)
    {
        OpNumber = N;
        OpNumberModified = true;
    }
}

CUSTOM_COMMAND_SIG(InputNumber0)
{ InputNumber(0); }

CUSTOM_COMMAND_SIG(InputNumber1)
{ InputNumber(1); }

CUSTOM_COMMAND_SIG(InputNumber2)
{ InputNumber(2); }

CUSTOM_COMMAND_SIG(InputNumber3)
{ InputNumber(3); }

CUSTOM_COMMAND_SIG(InputNumber4)
{ InputNumber(4); }

CUSTOM_COMMAND_SIG(InputNumber5)
{ InputNumber(5); }

CUSTOM_COMMAND_SIG(InputNumber6)
{ InputNumber(6); }

CUSTOM_COMMAND_SIG(InputNumber7)
{ InputNumber(7); }

CUSTOM_COMMAND_SIG(InputNumber8)
{ InputNumber(8); }

CUSTOM_COMMAND_SIG(InputNumber9)
{ InputNumber(9); }

fn BindInputNumbers
(Mapping* m, Command_Map* map)
{
    Bind(InputNumber0, KeyCode_0);
    Bind(InputNumber1, KeyCode_1);
    Bind(InputNumber2, KeyCode_2);
    Bind(InputNumber3, KeyCode_3);
    Bind(InputNumber4, KeyCode_4);
    Bind(InputNumber5, KeyCode_5);
    Bind(InputNumber6, KeyCode_6);
    Bind(InputNumber7, KeyCode_7);
    Bind(InputNumber8, KeyCode_8);
    Bind(InputNumber9, KeyCode_9);
}

fn SetupSharedKeyBindings
(Mapping* m, Command_Map* map)
{
    BindCore(default_startup, CoreCode_Startup);
    BindCore(default_try_exit, CoreCode_TryExit);
    BindCore(clipboard_record_clip, CoreCode_NewClipboardContents);
    BindTextInput(write_text_input);
    
    BindMouseWheel(mouse_wheel_scroll);
    BindMouseWheel(mouse_wheel_change_face_size, KeyCode_Control);
    BindMouse(click_set_cursor_and_mark, MouseCode_Left);
    BindMouseRelease(click_set_cursor, MouseCode_Left);
    BindCore(click_set_cursor_and_mark, CoreCode_ClickActivateView);
    
    BindMouseMove(click_set_cursor_if_lbutton);
    Bind(interactive_new,               KeyCode_N, KeyCode_Control);
    Bind(interactive_kill_buffer,       KeyCode_K, KeyCode_Control);
    Bind(interactive_switch_buffer,     KeyCode_I, KeyCode_Control);
    Bind(project_go_to_root_directory,  KeyCode_H, KeyCode_Control);
    Bind(save,                          KeyCode_S, KeyCode_Control);
    Bind(save_all_dirty_buffers,        KeyCode_S, KeyCode_Control, KeyCode_Shift);
    Bind(change_to_build_panel,         KeyCode_Period, KeyCode_Alt);
    Bind(close_build_panel,             KeyCode_Comma, KeyCode_Alt);
    Bind(goto_next_jump,                KeyCode_N, KeyCode_Alt);
    Bind(goto_prev_jump,                KeyCode_N, KeyCode_Alt, KeyCode_Shift);
    Bind(build_in_build_panel,          KeyCode_M, KeyCode_Alt);
    Bind(goto_first_jump,               KeyCode_M, KeyCode_Alt, KeyCode_Shift);
    Bind(toggle_filebar,                KeyCode_B, KeyCode_Alt);
    Bind(execute_any_cli,               KeyCode_Z, KeyCode_Alt);
    Bind(execute_previous_cli,          KeyCode_Z, KeyCode_Alt, KeyCode_Shift);
    Bind(quick_swap_buffer,             KeyCode_BackwardSlash, KeyCode_Alt);
    Bind(jump_to_last_point,            KeyCode_P, KeyCode_Control);
    Bind(list_all_functions_current_buffer_lister, KeyCode_I, KeyCode_Control, KeyCode_Shift);
    Bind(exit_4coder,          KeyCode_F4, KeyCode_Alt);
    Bind(project_fkey_command, KeyCode_F1);
    Bind(project_fkey_command, KeyCode_F2);
    Bind(page_up,                KeyCode_PageUp);
    Bind(page_down,              KeyCode_PageDown);
    Bind(toggle_fullscreen, KeyCode_F11);
}

fn SetupKeyBindings
(Mapping *mapping)
{
    NormalModeMapId = vars_save_string_lit("mapid_normal");
    InsertModeMapId = vars_save_string_lit("mapid_insert");
    DeleteModeMapId = vars_save_string_lit("mapid_delete");
	ChangeModeMapId = vars_save_string_lit("mapid_change");
	PanelModeMapId = vars_save_string_lit("mapid_panel");
    
    MappingScope();
    SelectMapping(mapping);
    
	// normal mode
    SelectMap(NormalModeMapId);
    SetupSharedKeyBindings(m, map);
    BindInputNumbers(m, map);
    
    Bind(reopen, KeyCode_F2);
    Bind(list_all_locations, KeyCode_F3);
    Bind(list_all_locations_of_identifier, KeyCode_F3, KeyCode_Control);
    Bind(list_all_substring_locations_case_insensitive, KeyCode_F3, KeyCode_Control, KeyCode_Shift);
    Bind(list_all_locations_of_selection, KeyCode_F3, KeyCode_Alt);
    Bind(query_replace, KeyCode_F5);
    Bind(query_replace_identifier, KeyCode_F5, KeyCode_Control);
    Bind(query_replace_selection, KeyCode_F5, KeyCode_Alt);
    Bind(goto_jump_at_cursor, KeyCode_F6);
    Bind(jump_to_definition_at_cursor, KeyCode_F6);
    Bind(jump_to_definition, KeyCode_F6, KeyCode_Control);
    Bind(open_matching_file_cpp, KeyCode_F7);
    Bind(toggle_line_numbers, KeyCode_F10);
    Bind(load_project, KeyCode_F12);
    Bind(place_in_scope, KeyCode_Equal);
    Bind(backspace_char, KeyCode_Backspace);
    Bind(delete_char, KeyCode_Delete);
    
    Bind(cursor_mark_swap, KeyCode_Q);
    Bind(MoveAlphaNumbericBoundaryRight, KeyCode_W);
    Bind(MoveAlphaNumbericOrCamelBoundaryRight, KeyCode_W, KeyCode_Control);
    Bind(select_surrounding_scope, KeyCode_E);
    Bind(redo, KeyCode_R);
    Bind(duplicate_line, KeyCode_T);
    Bind(copy, KeyCode_Y);
    Bind(undo, KeyCode_U);
    Bind(GoToInsertMode, KeyCode_I);
    Bind(command_lister, KeyCode_I, KeyCode_Control);
    Bind(NewLine, KeyCode_O);
    Bind(NewLine2, KeyCode_O, KeyCode_Control);
    Bind(NewLineStayingAtTheSamePlace, KeyCode_O, KeyCode_Alt);
    Bind(interactive_open_or_new, KeyCode_O, KeyCode_Control);
    Bind(paste_and_indent, KeyCode_P);
    Bind(open_in_other, KeyCode_P, KeyCode_Control);
    Bind(MoveUpToBlankLineEnd, KeyCode_LeftBracket);
    Bind(MoveDownToBlankLineEnd, KeyCode_RightBracket);
    
    Bind(MoveRightAndGoToInsertMode, KeyCode_A);
    Bind(delete_char, KeyCode_S);
    Bind(save_all_dirty_buffers, KeyCode_S, KeyCode_Control);
    Bind(GoToDeleteMode, KeyCode_D);
    Bind(SelectLine, KeyCode_F);
    Bind(goto_line, KeyCode_G);
    Bind(MoveLeft, KeyCode_H);
    Bind(seek_beginning_of_line, KeyCode_H, KeyCode_Control);
    Bind(MoveDown, KeyCode_J);
    Bind(MoveLineDown, KeyCode_J, KeyCode_Alt);
    Bind(MoveUp, KeyCode_K);
    Bind(MoveLineUp, KeyCode_K, KeyCode_Alt);
    Bind(MoveRight, KeyCode_L);
    Bind(seek_end_of_line, KeyCode_L, KeyCode_Control);
    Bind(comment_line_toggle, KeyCode_Semicolon);
    
    Bind(keyboard_macro_start_recording , KeyCode_Z, KeyCode_Shift);
    Bind(keyboard_macro_finish_recording, KeyCode_Z, KeyCode_Control);
    Bind(keyboard_macro_replay, KeyCode_Z);
    Bind(cut, KeyCode_X);
    Bind(GoToChangeMode, KeyCode_C);
    Bind(GoToPanelMode, KeyCode_V);
    Bind(MoveAlphaNumbericBoundaryLeft, KeyCode_B);
    Bind(MoveAlphaNumbericOrCamelBoundaryLeft, KeyCode_B, KeyCode_Control);
    Bind(set_mark, KeyCode_Space);
    Bind(goto_beginning_of_file, KeyCode_M);
    Bind(goto_end_of_file, KeyCode_N);
    Bind(ChangeActivePanel, KeyCode_Period, KeyCode_Control);
    Bind(ChangeActivePanelBackwards, KeyCode_Comma, KeyCode_Control);
    Bind(search, KeyCode_ForwardSlash);
    Bind(search_identifier, KeyCode_ForwardSlash, KeyCode_Control);
    
	// insert mode
    SelectMap(InsertModeMapId);
    SetupSharedKeyBindings(m, map);
    BindTextInput(write_text_and_auto_indent);
    Bind(GoToNormalMode, KeyCode_Escape);
    Bind(move_left, KeyCode_Left);
    Bind(move_right, KeyCode_Right);
    Bind(move_up, KeyCode_Up);
    Bind(move_down, KeyCode_Down);
    Bind(delete_char, KeyCode_Delete);
    Bind(backspace_char, KeyCode_Backspace);
    Bind(MoveUpToBlankLineEnd, KeyCode_Up, KeyCode_Control);
    Bind(MoveDownToBlankLineEnd, KeyCode_Down, KeyCode_Control);
    Bind(MoveAlphaNumbericBoundaryLeft,           KeyCode_Left, KeyCode_Control);
    Bind(MoveAlphaNumbericBoundaryRight,          KeyCode_Right, KeyCode_Control);
    Bind(MoveAlphaNumbericOrCamelBoundaryLeft,  KeyCode_Left, KeyCode_Alt);
    Bind(MoveAlphaNumbericOrCamelBoundaryRight, KeyCode_Right, KeyCode_Alt);
    Bind(word_complete, KeyCode_Tab);
    Bind(word_complete_drop_down, KeyCode_Tab, KeyCode_Control);
    Bind(WriteTodo, KeyCode_T, KeyCode_Alt);
    Bind(WriteTodoNow, KeyCode_T, KeyCode_Alt, KeyCode_Control);
    Bind(WriteNote, KeyCode_N, KeyCode_Alt);
    Bind(write_block, KeyCode_C, KeyCode_Alt);
    Bind(write_zero_struct, KeyCode_0, KeyCode_Alt);
    Bind(open_long_braces, KeyCode_LeftBracket, KeyCode_Shift);
    Bind(open_long_braces_semicolon, KeyCode_LeftBracket, KeyCode_Alt, KeyCode_Shift);
    Bind(open_long_braces_break, KeyCode_LeftBracket, KeyCode_Control, KeyCode_Shift);
    Bind(goto_beginning_of_file, KeyCode_PageUp, KeyCode_Control);
    Bind(goto_end_of_file, KeyCode_PageDown, KeyCode_Control);
    Bind(seek_beginning_of_line, KeyCode_Home);
    Bind(seek_end_of_line, KeyCode_End);
    
	// delete mode
    SelectMap(DeleteModeMapId);
    SetupSharedKeyBindings(m, map);
    BindInputNumbers(m, map);
    Bind(ModalDeleteDown, KeyCode_J);
    Bind(GoToNormalMode, KeyCode_Escape);
    Bind(ModalDeleteWordLeft, KeyCode_B);
    Bind(ModalDeleteWordRight, KeyCode_W);
    Bind(ModalDeleteLine, KeyCode_D);
    Bind(delete_current_scope, KeyCode_S);
    Bind(delete_range, KeyCode_A);
    Bind(ModalDeleteUp, KeyCode_K);
    
	// change mode
    SelectMap(ChangeModeMapId);
    SetupSharedKeyBindings(m, map);
    BindInputNumbers(m, map);
    Bind(GoToNormalMode, KeyCode_Escape);
    Bind(ModalChangeLine, KeyCode_C);
    Bind(ModalChangeWordRight, KeyCode_W);
    Bind(ModalChangeWordLeft, KeyCode_B);
    Bind(ModalChangeUp, KeyCode_K);
    Bind(ModalChangeDown, KeyCode_J);
    
    // panel mode
    SelectMap(PanelModeMapId);
    SetupSharedKeyBindings(m, map);
    Bind(GoToNormalMode, KeyCode_Escape);
    Bind(ChangeActivePanel, KeyCode_L);
    Bind(ChangeActivePanel, KeyCode_V);
    Bind(ChangeActivePanelBackwards, KeyCode_H);
    Bind(SwapPanels, KeyCode_S);
    Bind(ViewBufferOtherPanel, KeyCode_D);
    Bind(OpenPanelVSplit, KeyCode_BackwardSlash);
    Bind(OpenPanelHSplit, KeyCode_Minus);
    Bind(ClosePanel, KeyCode_0);
    
    // make default maps inherit mode maps
    String_ID GlobalMapId = vars_save_string_lit("keys_global");
    String_ID FileMapId = vars_save_string_lit("keys_file");
    String_ID CodeMapId  = vars_save_string_lit("keys_code");
    
	SelectMap(GlobalMapId);
	SetupSharedKeyBindings(m, map);
    
    SelectMap(FileMapId);
    ParentMap(NormalModeMapId);
    
    SelectMap(CodeMapId);
    ParentMap(NormalModeMapId);
}

void
custom_layer_init
(Application_Links *App)
{
    Thread_Context* tctx = get_thread_context(App);
    
    default_framework_init(App);
    set_all_default_hooks(App);
    mapping_init(tctx, &framework_mapping);
    
    SetupKeyBindings(&framework_mapping);  
	SetNormalModeColors();
}

#endif

/* TODO:
Make undo work well with all commands
Replace character
Add lister for putting code bookmarks so you can jump around
Modify the indenter
Saving on switching to normal mode, Need to have possibility of turning that off
*/

