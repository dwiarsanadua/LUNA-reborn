--[[
  Quest 840: I want to eat 2
  Level Required: 84
  NPC Start: 98, NPC Complete: 123
  Prerequisites: [839]
  Rewards: EXP=1404408
]]

local fsm = require('fsm_engine')

local quest_840 = fsm:new({
    id = 840,
    name = "I want to eat 2",
    level_required = 84,
    npc_start = 98,
    npc_complete = 123,
    prerequisites = {839},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 840: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 840: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 840: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 840: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 123,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 840: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 840: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 123,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 840: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 840: Rewards given")
                self:give_rewards({
                    exp = 1404408,
                    items = {
                        { item_id = 21000010, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_840