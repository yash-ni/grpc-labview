//---------------------------------------------------------------------
//---------------------------------------------------------------------
#pragma once

//---------------------------------------------------------------------
//---------------------------------------------------------------------
#include <message_value.h>
#include <message_metadata.h>
#include <google/protobuf/message.h>

using namespace google::protobuf::internal;

namespace grpc_labview 
{
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    class LVMessage : public google::protobuf::Message, public gRPCid
    {
    public:
        LVMessage(std::shared_ptr<MessageMetadata> metadata);
        LVMessage(std::shared_ptr<MessageMetadata> metadata, bool use_hardcoded_parse, bool skipCopyOnFirstParse);
        ~LVMessage();

        google::protobuf::UnknownFieldSet& UnknownFields();

        Message* New(google::protobuf::Arena* arena) const override;
        void SharedCtor();
        void SharedDtor();
        void ArenaDtor(void* object);
        void RegisterArenaDtor(google::protobuf::Arena*);

        void Clear()  final;
        bool IsInitialized() const final;

        const char* _InternalParse(const char *ptr, google::protobuf::internal::ParseContext *ctx)  override final;
        google::protobuf::uint8* _InternalSerialize(google::protobuf::uint8* target, google::protobuf::io::EpsCopyOutputStream* stream) const override final;
        void SetCachedSize(int size) const final;
        int GetCachedSize(void) const final;
        size_t ByteSizeLong() const final;
        
        void MergeFrom(const google::protobuf::Message &from) final;
        void MergeFrom(const LVMessage &from);
        void CopyFrom(const google::protobuf::Message &from) final;
        void CopyFrom(const LVMessage &from);
        void InternalSwap(LVMessage *other);
        google::protobuf::Metadata GetMetadata() const final;

        bool ParseFromByteBuffer(const grpc::ByteBuffer& buffer);
        std::unique_ptr<grpc::ByteBuffer> SerializeToByteBuffer();

    public:
        std::map<int, std::shared_ptr<LVMessageValue>> _values;
        std::shared_ptr<MessageMetadata> _metadata;
        std::unordered_map<std::string, uint32_t> _repeatedField_continueIndex;
        std::unordered_map<std::string, google::protobuf::RepeatedField<char>> _repeatedMessageValuesMap;
        std::unordered_map<std::string, google::protobuf::RepeatedField<std::string>> _repeatedStringValuesMap;
        // std::vector<uint64_t> _messageValues;
        //std::vector<char> _repeatedMessageValues;
        bool _use_hardcoded_parse;
        bool _skipCopyOnFirstParse;

        void setLVClusterHandle(const char* lvClusterHandle) {
            _LVClusterHandle = lvClusterHandle;
        };

        const char* getLVClusterHandleSharedPtr() {
            return _LVClusterHandle;
        };

    private:
        mutable google::protobuf::internal::CachedSize _cached_size_;
        google::protobuf::UnknownFieldSet _unknownFields;
        const char* _LVClusterHandle;

        const char *ParseBoolean(const MessageElementMetadata& fieldInfo, uint32_t index, const char *ptr, google::protobuf::internal::ParseContext *ctx);
        const char *ParseInt32(const MessageElementMetadata& fieldInfo, uint32_t index, const char *ptr, google::protobuf::internal::ParseContext *ctx);
        const char *ParseUInt32(const MessageElementMetadata& fieldInfo, uint32_t index, const char *ptr, google::protobuf::internal::ParseContext *ctx);
        const char *ParseEnum(const MessageElementMetadata& fieldInfo, uint32_t index, const char *ptr, google::protobuf::internal::ParseContext *ctx);
        const char *ParseInt64(const MessageElementMetadata& fieldInfo, uint32_t index, const char *ptr, google::protobuf::internal::ParseContext *ctx);
        const char *ParseUInt64(const MessageElementMetadata& fieldInfo, uint32_t index, const char *ptr, google::protobuf::internal::ParseContext *ctx);
        const char *ParseFloat(const MessageElementMetadata& fieldInfo, uint32_t index, const char *ptr, google::protobuf::internal::ParseContext *ctx);
        const char *ParseDouble(const MessageElementMetadata& fieldInfo, uint32_t index, const char *ptr, google::protobuf::internal::ParseContext *ctx);
        const char* ParseSInt32(const MessageElementMetadata& fieldInfo, uint32_t index, const char* ptr, google::protobuf::internal::ParseContext* ctx);
        const char* ParseSInt64(const MessageElementMetadata& fieldInfo, uint32_t index, const char* ptr, google::protobuf::internal::ParseContext* ctx);
        const char* ParseFixed32(const MessageElementMetadata& fieldInfo, uint32_t index, const char* ptr, google::protobuf::internal::ParseContext* ctx);
        const char* ParseFixed64(const MessageElementMetadata& fieldInfo, uint32_t index, const char* ptr, google::protobuf::internal::ParseContext* ctx);
        const char* ParseSFixed32(const MessageElementMetadata& fieldInfo, uint32_t index, const char* ptr, google::protobuf::internal::ParseContext* ctx);
        const char* ParseSFixed64(const MessageElementMetadata& fieldInfo, uint32_t index, const char* ptr, google::protobuf::internal::ParseContext* ctx);
        const char *ParseString(unsigned int tag, const MessageElementMetadata& fieldInfo, uint32_t index, const char *ptr, google::protobuf::internal::ParseContext *ctx);
        const char *ParseBytes(unsigned int tag, const MessageElementMetadata& fieldInfo, uint32_t index, const char *ptr, google::protobuf::internal::ParseContext *ctx);
        const char *ParseNestedMessage(google::protobuf::uint32 tag, const MessageElementMetadata& fieldInfo, uint32_t index, const char *ptr, google::protobuf::internal::ParseContext *ctx);
        bool ExpectTag(google::protobuf::uint32 tag, const char* ptr);
    };

    template <typename MessageType>
    class SinglePassMessageParser {
    private:
        LVMessage& _message;
    public:
        // Constructor and other necessary member functions
        SinglePassMessageParser(LVMessage& message) : _message(message) {}

        // Parse and copy message in a single pass.
        const char* ParseAndCopyMessage(const MessageElementMetadata& fieldInfo, uint32_t index, const char *ptr, ParseContext *ctx) {
            // get the LVClusterHandle
            auto _lv_ptr = reinterpret_cast<const char*>(_message.getLVClusterHandleSharedPtr()) + fieldInfo.clusterOffset;

            if (fieldInfo.isRepeated)
            {
                // Read the repeated elements into a temporary vector
                uint64_t numElements;
                auto v = std::make_shared<LVRepeatedMessageValue<MessageType>>(index);
                ptr = PackedMessageType(ptr, ctx, index, &(v->_value));
                numElements = v->_value.size();
                
                // copy into LVCluster
                if (numElements != 0)
                {
                    NumericArrayResize(0x08, 1, reinterpret_cast<void *>(const_cast<char *>(_lv_ptr)), numElements);
                    auto array = *(LV1DArrayHandle*)_lv_ptr;
                    (*array)->cnt = numElements;
                    auto byteCount = numElements * sizeof(MessageType);
                    std::memcpy((*array)->bytes<MessageType>(), v->_value.data(), byteCount);
                }
            }
            else
            {
                ptr = ReadMessageType(ptr, reinterpret_cast<MessageType*>(const_cast<char *>(_lv_ptr)));
            }
            return ptr;
        }

        const char* ReadMessageType(const char* ptr, MessageType* lv_ptr)
        {
            return nullptr;
        }

        const char* PackedMessageType(const char* ptr, ParseContext* ctx, int index, google::protobuf::RepeatedField<MessageType>* value)
        {
            return nullptr;
        }
    };

    const char* SinglePassMessageParser<int32_t>::ReadMessageType(const char* ptr, int32_t* lv_ptr)
    {
        return ReadINT32(ptr, lv_ptr);
    }
    
    const char* SinglePassMessageParser<int32_t>::PackedMessageType(const char* ptr, ParseContext* ctx, int index, google::protobuf::RepeatedField<int32_t>* value)
    {
        return PackedInt32Parser(value, ptr, ctx);
    }

    const char* SinglePassMessageParser<uint32_t>::ReadMessageType(const char* ptr, uint32_t* lv_ptr)
    {
        return ReadUINT32(ptr, lv_ptr);
    }

    const char* SinglePassMessageParser<uint32_t>::PackedMessageType(const char* ptr, ParseContext* ctx, int index, google::protobuf::RepeatedField<uint32_t>* value)
    {
        return PackedUInt32Parser(value, ptr, ctx);
    }

    const char* SinglePassMessageParser<uint64_t>::ReadMessageType(const char* ptr, uint64_t* lv_ptr)
    {
        return ReadUINT64(ptr, lv_ptr);
    }
    
    const char* SinglePassMessageParser<uint64_t>::PackedMessageType(const char* ptr, ParseContext* ctx, int index, google::protobuf::RepeatedField<uint64_t>* value)
    {
        return PackedUInt64Parser(value, ptr, ctx);
    }
}
