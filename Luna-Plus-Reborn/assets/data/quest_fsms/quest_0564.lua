--[[
  Quest 564: Quest 564
  Level Required: 1
  NPC Start: 199, NPC Complete: 199
  Rewards: EXP=1000
]]

local fsm = require('fsm_engine')

local quest_564 = fsm:new({
    id = 564,
    name = "Quest 564",
    level_required = 1,
    npc_start = 199,
    npc_complete = 199,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 564: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 564: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 564: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 18,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 564: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 7,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 564: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 1,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 564: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 5,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 564: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 564: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 564: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 564: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 564: Rewards given")
                self:give_rewards({
                    exp = 1000,
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

return quest_564