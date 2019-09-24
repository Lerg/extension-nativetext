#if defined(DM_PLATFORM_IOS)

#include <dmsdk/sdk.h>
#include "extension.h"

#import "ios/utils.h"

// Using proper Objective-C object for main extension entity.
@interface ExtensionInterface : NSObject
@end

@implementation ExtensionInterface {
    bool is_initialized;
    LuaScriptListener *script_listener;
}

static NSString *const ADJUST = @"adjust";
static NSString *const EVENT_PHASE = @"phase";
static NSString *const EVENT_INIT = @"init";
static NSString *const EVENT_IS_ERROR = @"is_error";
static NSString *const EVENT_ERROR_MESSAGE = @"error_message";

static ExtensionInterface *extension_instance;
int EXTENSION_INIT(lua_State *L) {return [extension_instance init_:L];}

-(id)init:(lua_State*)L {
	self = [super init];

    is_initialized = false;
	script_listener = [LuaScriptListener new];
    script_listener.listener = LUA_REFNIL;
	script_listener.script_instance = LUA_REFNIL;

	return self;
}

-(bool)check_is_initialized {
	if (is_initialized) {
		return true;
	} else {
		dmLogInfo("The extension is not initialized.");
		return false;
	}
}

# pragma mark - Lua functions -

// adjust.init(params)
-(int)init_:(lua_State*)L {
	[Utils check_arg_count:L count:1];
	if (is_initialized) {
		dmLogInfo("The extension is already initialized.");
		return 0;
	}

    Scheme *scheme = [[Scheme alloc] init];
    [scheme function:@"listener"];

    Table *params = [[Table alloc] init:L index:1];
    [params parse:scheme];

	[Utils delete_ref_if_not_nil:script_listener.listener];
	[Utils delete_ref_if_not_nil:script_listener.script_instance];
    script_listener.listener = [params get_function:@"listener" default:LUA_REFNIL];
	dmScript::GetInstance(L);
	script_listener.script_instance = [Utils new_ref:L];

    is_initialized = true;
    NSMutableDictionary *event = [Utils new_event:ADJUST];
    event[EVENT_PHASE] = EVENT_INIT;
    event[EVENT_IS_ERROR] = @((bool)false);
    [Utils dispatch_event:script_listener event:event];
	return 0;
}

-(int)_new:(lua_State*)L {
	[Utils check_arg_count:L count:1];
	if (![self check_is_initialized]) {
		return 0;
	}

	Scheme *scheme = [[Scheme alloc] init];
	[scheme string:@"token"];
	[scheme number:@"revenue"];
	[scheme string:@"currency"];
	[scheme string:@"transaction_id"];
	[scheme string:@"callback_id"];
	[scheme table:@"callback_parameters"];
	[scheme string:@"callback_parameters.#"];
	[scheme table:@"partner_parameters"];
	[scheme string:@"partner_parameters.#"];

	Table *params = [[Table alloc] init:L index:1];
	[params parse:scheme];

	NSString *token = [params get_string_not_null:@"token"];
	NSNumber *revenue = [params get_double:@"revenue"];
	NSString *currency = [params get_string:@"currency"];
	NSString *transaction_id = [params get_string:@"transaction_id"];
	NSString *callback_id = [params get_string:@"callback_id"];
	NSDictionary *callback_parameters = [params get_table:@"callback_parameters"];
	NSDictionary *partner_parameters = [params get_table:@"partner_parameters"];

	return 0;
}

@end

#pragma mark - Defold lifecycle -

void EXTENSION_INITIALIZE(lua_State *L) {
	extension_instance = [[ExtensionInterface alloc] init:L];
}

void EXTENSION_UPDATE(lua_State *L) {
	[Utils execute_tasks:L];
}

void EXTENSION_APP_ACTIVATE(lua_State *L) {
}

void EXTENSION_APP_DEACTIVATE(lua_State *L) {
}

void EXTENSION_FINALIZE(lua_State *L) {
    extension_instance = nil;
}

#endif
