--[[
  Quest 803: [Party] Gathering Evidence
  Level Required: 68
  NPC Start: 117, NPC Complete: 117
  Rewards: EXP=1256838, Gold=32313
]]

local fsm = require('fsm_engine')

local quest_803 = fsm:new({
    id = 803,
    name = "[Party] Gathering Evidence",
    level_required = 68,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 803: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 803: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 803: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 89,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 803: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 803: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 803: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 803: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 803: Rewards given")
                self:give_rewards({
                    exp = 1256838,
                    gold = 32313,
                    items = {
                        { item_id = 381, count = 30 },
                        { item_id = 30000686, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_803