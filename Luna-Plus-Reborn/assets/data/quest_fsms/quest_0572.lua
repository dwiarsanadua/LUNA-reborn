--[[
  Quest 572: Quest 572
  Level Required: 81
  NPC Start: 199, NPC Complete: 199
  Rewards: EXP=1500000
]]

local fsm = require('fsm_engine')

local quest_572 = fsm:new({
    id = 572,
    name = "Quest 572",
    level_required = 81,
    npc_start = 199,
    npc_complete = 199,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 572: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 572: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 572: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 271,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 572: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 275,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 572: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 281,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 572: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 283,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 572: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 572: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 572: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 572: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 572: Rewards given")
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

return quest_572