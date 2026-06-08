--[[
  Quest 104: Taste of Home
  Level Required: 10
  NPC Start: 14, NPC Complete: 52
  Rewards: EXP=602, Gold=750
]]

local fsm = require('fsm_engine')

local quest_104 = fsm:new({
    id = 104,
    name = "Taste of Home",
    level_required = 10,
    npc_start = 14,
    npc_complete = 52,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 104: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 52,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 104: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 104: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 52,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 104: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 14,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 104: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 104: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 52,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 104: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 104: Rewards given")
                self:give_rewards({
                    exp = 602,
                    gold = 750,
                    items = {
                        { item_id = 46, count = 1 },
                        { item_id = 21000007, count = 8 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_104