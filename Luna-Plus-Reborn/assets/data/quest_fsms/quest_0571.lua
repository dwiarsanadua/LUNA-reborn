--[[
  Quest 571: Quest 571
  Level Required: 71
  NPC Start: 199, NPC Complete: 199
  Rewards: EXP=1000000
]]

local fsm = require('fsm_engine')

local quest_571 = fsm:new({
    id = 571,
    name = "Quest 571",
    level_required = 71,
    npc_start = 199,
    npc_complete = 199,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 571: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 571: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 571: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 104,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 571: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 252,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 571: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 91,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 571: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 273,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 571: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 571: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 571: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 571: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 571: Rewards given")
                self:give_rewards({
                    exp = 1000000,
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

return quest_571