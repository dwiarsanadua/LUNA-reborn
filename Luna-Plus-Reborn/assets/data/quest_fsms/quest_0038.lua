--[[
  Quest 38: Delivering the reply
  Level Required: 48
  NPC Start: 4, NPC Complete: 12
  Prerequisites: [6]
  Rewards: EXP=1044, Gold=795
]]

local fsm = require('fsm_engine')

local quest_38 = fsm:new({
    id = 38,
    name = "Delivering the reply",
    level_required = 48,
    npc_start = 4,
    npc_complete = 12,
    prerequisites = {6},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 38: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 12,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 38: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 38: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 12,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 38: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 4,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 38: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 38: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 12,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 38: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 38: Rewards given")
                self:give_rewards({
                    exp = 1044,
                    gold = 795,
                    items = {
                        { item_id = 16, count = 1 },
                        { item_id = 16, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_38