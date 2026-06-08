--[[
  Quest 877: [Party] Monster Book
  Level Required: 93
  NPC Start: 91, NPC Complete: 91
  Rewards: EXP=5911530, Gold=61188
]]

local fsm = require('fsm_engine')

local quest_877 = fsm:new({
    id = 877,
    name = "[Party] Monster Book",
    level_required = 93,
    npc_start = 91,
    npc_complete = 91,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 877: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 877: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 877: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 287,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 877: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 288,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 877: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 289,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 877: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 877: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 877: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 877: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 877: Rewards given")
                self:give_rewards({
                    exp = 5911530,
                    gold = 61188,
                    items = {
                        { item_id = 21000282, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_877