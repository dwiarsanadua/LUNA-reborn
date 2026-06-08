--[[
  Quest 568: Quest 568
  Level Required: 41
  NPC Start: 199, NPC Complete: 199
  Rewards: EXP=250000
]]

local fsm = require('fsm_engine')

local quest_568 = fsm:new({
    id = 568,
    name = "Quest 568",
    level_required = 41,
    npc_start = 199,
    npc_complete = 199,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 568: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 568: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 568: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 44,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 568: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 38,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 568: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 17,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 568: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 13,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 568: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 568: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 568: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 568: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 568: Rewards given")
                self:give_rewards({
                    exp = 250000,
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

return quest_568