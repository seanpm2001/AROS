/*
    Copyright (C) 2020-2022, The AROS Development Team. All rights reserved.
*/

#include <aros/debug.h>

#include <proto/exec.h>
#include <devices/trackdisk.h>
#include <devices/newstyle.h>
#include <asm/io.h>
#include <hidd/pci.h>
#include <interface/Hidd_PCIDriver.h>

#include <string.h>

#if defined(__AROSEXEC_SMP__)
#include <proto/kernel.h>
#endif

#include "nvme_intern.h"
#include "nvme_hw.h"

/*
    IO Queue Support Functions
*/

void nvme_complete_ioevent(struct nvme_queue *nvmeq, struct nvme_completion *cqe)
{
    D(bug ("[NVME:IOQ] %s(0x%p)\n", __func__, cqe);)
    if (nvmeq->cehandlers[cqe->command_id])
    {
        nvmeq->cehandlers[cqe->command_id]->ceh_Reply = TRUE;
        nvmeq->cehandlers[cqe->command_id]->ceh_Result = AROS_LE2LONG(cqe->result);
        nvmeq->cehandlers[cqe->command_id]->ceh_Status = (AROS_LE2WORD(cqe->status) >> 1) & ~(3 << 2);
        D(bug ("[NVME:IOQ] %s: Signaling 0x%p (%08x)\n", __func__, nvmeq->cehandlers[cqe->command_id]->ceh_Task, nvmeq->cehandlers[cqe->command_id]->ceh_SigSet);)
        Signal(nvmeq->cehandlers[cqe->command_id]->ceh_Task, nvmeq->cehandlers[cqe->command_id]->ceh_SigSet);
    }
}

int nvme_submit_iocmd(struct completionevent_handler *ce,
                                    struct nvme_queue *nvmeq,
                                    struct nvme_command *cmd,
                                    struct completionevent_handler *handler)
{
    int retval;

    D(bug ("[NVME:IOQ] %s(0x%p, 0x%p)\n", __func__, cmd);)

    handler->ceh_Reply = FALSE;
    cmd->common.op.command_id = nvme_alloc_cmdid(nvmeq);
    nvmeq->cehooks[cmd->common.op.command_id] = nvme_complete_ioevent;
    nvmeq->cehandlers[cmd->common.op.command_id] = &ce[cmd->common.op.command_id];
    CopyMem(handler, &ce[cmd->common.op.command_id], sizeof(struct completionevent_handler));
    retval = nvme_submit_cmd(nvmeq, cmd);

    return retval;
}
