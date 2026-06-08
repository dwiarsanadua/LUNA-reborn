--[[
  Quest 570: Quest 570
  Level Required: 61
  NPC Start: 199, NPC Complete: 199
  Rewards: EXP=750000
]]

local fsm = require('fsm_engine')

local quest_570 = fsm:new({
    id = 570,
    name = "Quest 570",
    level_required = 61,
    npc_start = 199,
    npc_complete = 199,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 570: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 570: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 570: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 253,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 570: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 249,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 570: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 88,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 570: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 104,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 570: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 570: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 570: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 570: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 570: Rewards given")
                self:give_rewards({
                    exp = 750000,
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

return quest_570