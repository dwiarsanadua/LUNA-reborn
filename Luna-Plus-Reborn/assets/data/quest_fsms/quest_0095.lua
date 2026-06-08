--[[
  Quest 95: Memorial Service
  Level Required: 10
  NPC Start: 16, NPC Complete: 19
  Rewards: EXP=602, Gold=750
]]

local fsm = require('fsm_engine')

local quest_95 = fsm:new({
    id = 95,
    name = "Memorial Service",
    level_required = 10,
    npc_start = 16,
    npc_complete = 19,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 95: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 16,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 95: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 95: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 16,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 95: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 95: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 95: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 19,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 95: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 95: Rewards given")
                self:give_rewards({
                    exp = 602,
                    gold = 750,
                    items = {
                        { item_id = 38, count = 1 },
                        { item_id = 38, count = 1 },
                        { item_id = 21000007, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_95