--[[
  Quest 566: Quest 566
  Level Required: 21
  NPC Start: 199, NPC Complete: 199
  Rewards: EXP=15000
]]

local fsm = require('fsm_engine')

local quest_566 = fsm:new({
    id = 566,
    name = "Quest 566",
    level_required = 21,
    npc_start = 199,
    npc_complete = 199,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 566: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 566: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 566: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 8,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 566: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 19,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 566: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 48,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 566: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 42,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 566: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 566: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 566: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 566: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 566: Rewards given")
                self:give_rewards({
                    exp = 15000,
                    items = {
                        { item_id = 21001049, count = 1 },
                        { item_id = 21001048, count = 1 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_566