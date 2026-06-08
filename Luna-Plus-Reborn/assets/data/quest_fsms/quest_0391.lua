--[[
  Quest 391: [Special][Quest]A study of secret medicine
  Level Required: 60
  NPC Start: 92, NPC Complete: 92
  Prerequisites: [390]
  Rewards: EXP=583952, Gold=16000
]]

local fsm = require('fsm_engine')

local quest_391 = fsm:new({
    id = 391,
    name = "[Special][Quest]A study of secret medicine",
    level_required = 60,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {390},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 391: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 391: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 391: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 258,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 391: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 391: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 391: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 391: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 391: Rewards given")
                self:give_rewards({
                    exp = 583952,
                    gold = 16000,
                    items = {
                        { item_id = 297, count = 1 },
                        { item_id = 297, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_391