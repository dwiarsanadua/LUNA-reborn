--[[
  Quest 757: [Quest] High Priest Luna
  Level Required: 0
  NPC Start: 7, NPC Complete: 400
  Prerequisites: [756]
  Rewards: Gold=5000
]]

local fsm = require('fsm_engine')

local quest_757 = fsm:new({
    id = 757,
    name = "[Quest] High Priest Luna",
    level_required = 0,
    npc_start = 7,
    npc_complete = 400,
    prerequisites = {756},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 757: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 400,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 757: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 757: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 400,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 757: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 757: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 757: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 757: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 400,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 757: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 757: Rewards given")
                self:give_rewards({
                    gold = 5000,
                    items = {
                        { item_id = 21000245, count = 1 },
                        { item_id = 21001530, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_757