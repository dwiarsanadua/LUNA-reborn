--[[
  Quest 583: Quest 583
  Level Required: 81
  NPC Start: 199, NPC Complete: 199
  Prerequisites: [572]
  Rewards: EXP=1500000
]]

local fsm = require('fsm_engine')

local quest_583 = fsm:new({
    id = 583,
    name = "Quest 583",
    level_required = 81,
    npc_start = 199,
    npc_complete = 199,
    prerequisites = {572},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 583: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 583: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 583: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 271,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 583: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 275,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 583: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 281,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 583: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 283,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 583: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 583: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 583: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 583: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 583: Rewards given")
                self:give_rewards({
                    exp = 1500000,
                    items = {
                        { item_id = 21001049, count = 1 },
                        { item_id = 21001048, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_583