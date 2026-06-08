--[[
  Quest 585: Quest 585
  Level Required: 101
  NPC Start: 199, NPC Complete: 199
  Prerequisites: [574]
  Rewards: EXP=5000000
]]

local fsm = require('fsm_engine')

local quest_585 = fsm:new({
    id = 585,
    name = "Quest 585",
    level_required = 101,
    npc_start = 199,
    npc_complete = 199,
    prerequisites = {574},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 585: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 585: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 585: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 320,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 585: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 293,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 585: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 294,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 585: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 325,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 585: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 585: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 585: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 585: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 585: Rewards given")
                self:give_rewards({
                    exp = 5000000,
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

return quest_585