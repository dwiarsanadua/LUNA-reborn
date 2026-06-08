--[[
  Quest 132: Family Values
  Level Required: 5
  NPC Start: 3, NPC Complete: 49
  Rewards: EXP=93
]]

local fsm = require('fsm_engine')

local quest_132 = fsm:new({
    id = 132,
    name = "Family Values",
    level_required = 5,
    npc_start = 3,
    npc_complete = 49,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 132: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 3,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 132: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 132: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 132: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 49,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 132: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 132: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 49,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 132: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 132: Rewards given")
                self:give_rewards({
                    exp = 93,
                    items = {
                        { item_id = 30000824, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_132