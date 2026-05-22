#include "../../core/api/BackendDev.h"

namespace
{
    /**
     * @brief reality modify this class into realy handler mainly, this is just a template for demo and testing purpose
     * 
     */
class ReadTemplateHandler final : public backend_core::v1::domain::QueryHandler
{
protected:
    backend_core::v1::TaskEnvelope ExecuteQuery(const backend_core::v1::TaskRequest& request) const override
    {
        backend_core::v1::ContentItem item;
        item.uuid = request.content.uuid != "" ? request.content.uuid : "read-template-id";
        item.title = request.content.title != "" ? request.content.title : "read-template-title";
        item.body = request.content.body != "" ? request.content.body : "read-template-body";
        item.updatedAt = backend_core::v1::domain::CurrentUnixTimeSec();
        item.version = request.content.version > 0 ? request.content.version : 1;

        return backend_core::v1::domain::BuildSuccessWithItemEnvelope(request, item, "read_template_ok");
    }
};

backend_core::v1::TaskEnvelope HandleReadTemplateEntry(const backend_core::v1::TaskRequest& request)
{
    return backend_core::v1::domain::InvokeFrameworkHandler<ReadTemplateHandler>(request);
}

AUTO_REGISTER(
    "template",
    "read",
    "get",
    HandleReadTemplateEntry);
}
