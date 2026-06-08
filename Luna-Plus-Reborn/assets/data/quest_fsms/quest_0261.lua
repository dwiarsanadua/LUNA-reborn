--[[
  Quest 261: [B]-Class Compound Water Sample
  Level Required: 150
  NPC Start: 24, NPC Complete: 24
]]

local fsm = require('fsm_engine')

local quest_261 = fsm:new({
    id = 261,
    name = "[B]-Class Compound Water Sample",
    level_required = 150,
    npc_start = 24,
    npc_complete = 24,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 261: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 261: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 261: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 261: NPC talk objective met")
                    end
                },
                {
                    trigger = "any",
                    target = "COMPLETE",
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 261: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 24,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 261: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 261: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21000155, count = 1 },
                        { item_id = 21000010, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_261