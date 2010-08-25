#ifndef _ofxsMessage_H_
#define _ofxsMessage_H_

namespace OFX
{
namespace Message
{
enum EMessageReply
{
	eMessageReplyOK,
	eMessageReplyYes,
	eMessageReplyNo,
	eMessageReplyFailed
};

enum EMessageType
{
	eMessageFatal,
	eMessageError,
	eMessageMessage,
	eMessageLog,
	eMessageQuestion
};
};
};

#endif

