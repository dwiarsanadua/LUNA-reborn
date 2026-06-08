--[[
  Quest 584: Quest 584
  Level Required: 91
  NPC Start: 199, NPC Complete: 199
  Prerequisites: [573]
  Rewards: EXP=3000000
]]

local fsm = require('fsm_engine')

local quest_584 = fsm:new({
    id = 584,
    name = "Quest 584",
    level_required = 91,
    npc_start = 199,
    npc_complete = 199,
    prerequisites = {573},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 584: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 584: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 584: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 283,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 584: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 287,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 584: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 370,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 584: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 293,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 584: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 584: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 584: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 199,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 584: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 584: Rewards given")
                self:give_rewards({
                    exp = 3000000,
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

return quest_584