--[[
  Quest 751: [Quest]Operating the farm is FUN!
  Level Required: 8
  NPC Start: 49, NPC Complete: 63
  Rewards: EXP=263
]]

local fsm = require('fsm_engine')

local quest_751 = fsm:new({
    id = 751,
    name = "[Quest]Operating the farm is FUN!",
    level_required = 8,
    npc_start = 49,
    npc_complete = 63,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 751: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 49,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 751: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 751: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 49,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 751: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 63,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 751: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 751: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 63,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 751: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 751: Rewards given")
                self:give_rewards({
                    exp = 263,
                    items = {
                        { item_id = 21000060, count = 1 },
                        { item_id = 21000067, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_751